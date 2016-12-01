/*
MIT License

Copyright (c) 2016 Erik Thomasson Forsberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "SN76489.h"

SN76489::SN76489(const uint16 _sampleRate, const int _clockSpeed)
{
	m_SampleRate = _sampleRate;
	m_Clock = _clockSpeed;

	m_CyclesPerSample = (double)m_Clock / (double)CLOCK_DIVIDER / (double)m_SampleRate;
	m_InvCycles = 1.0 / m_CyclesPerSample;
	m_CycleCount = m_CyclesPerSample;
}

SN76489::~SN76489()
{
}

void SN76489::write(const uint8 _data) {
	bool first = (_data & 128);
	if (first) {
		m_CurrentReg = (_data >> 5) & 3;
		m_CurrentType = (_data >> 4) & 1;
	}

	if (m_CurrentType) {
		m_Attn[m_CurrentReg] = _data & 0x0F;
	}
	else if (first && m_CurrentReg == 3) {
		m_FreqReg[3] = _data & 7;
		m_NoiseSR = 0x8000;
	}
	else if (first) {
		m_FreqReg[m_CurrentReg] = (m_FreqReg[m_CurrentReg] & 0x3F0) | (_data & 0x0F);
	}
	else {
		m_CountReg[m_CurrentReg] = m_FreqReg[m_CurrentReg] = (m_FreqReg[m_CurrentReg] & 0x0F) | ((_data & 0x3F) << 4);
	}
}

int16 SN76489::render() {
	while (m_CycleCount > 0) {
		for (uint8 i = 0; i < 4; i++) {
			m_CountReg[i]--;
			if (!m_CountReg[i]) {
				if (i < 3) {
					m_CountReg[i] = m_FreqReg[i];
					m_FlipFlop[i] = !m_FlipFlop[i];
				}
				else {
					uint8 nf = m_FreqReg[3] & 3;
					uint8 fb = (m_FreqReg[3] >> 2) & 1;
					m_CountReg[3] = nf == 3 ? m_FreqReg[2] : (0x10 << nf);

					m_NoiseSR = (m_NoiseSR >> 1) | ((fb ? parity(m_NoiseSR & NOISE_TAPPED) : m_NoiseSR & 1) << 15);
					m_FlipFlop[3] = (m_NoiseSR & 1);
				}
			}
		}

		m_CycleCount--;
	}

	m_CycleCount += m_CyclesPerSample;

	return getVolume(0) + getVolume(1) + getVolume(2) + getVolume(3);
}

inline uint16 SN76489::parity(uint16 _val) const {
	_val ^= _val >> 8;
	_val ^= _val >> 4;
	_val ^= _val >> 2;
	_val ^= _val >> 1;
	_val &= 1;
	return _val;
}

inline uint16 SN76489::getVolume(uint8 _chn) const {
	return (m_FlipFlop[_chn] ? 1 : -1) * volume_table[m_Attn[_chn]];
}