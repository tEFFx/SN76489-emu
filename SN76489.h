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

#pragma once

#define CLOCK_DIVIDER 16
#define NOISE_TAPPED 0x9

class SN76489
{
public:
	SN76489(const uint16, const int);
	~SN76489();

	void write(const uint8);
	int16 render();

private:
	uint16 parity(uint16) const;
	uint16 getVolume(uint8) const;

	const uint8 noise_table[3] = { 0x10, 0x20, 0x40 };
	const short volume_table[16] = {
		8191, 6507, 5168, 4105, 3261, 2590, 2057, 
		1642, 1298, 1031, 819, 650, 516, 410, 326, 0
	};

	int32 m_Clock;
	uint16 m_SampleRate;

	double m_CyclesPerSample;
	double m_CycleCount;
	double m_InvCycles;

	uint16 m_FreqReg[4];
	uint16 m_CountReg[4];
	uint8 m_Attn[4];
	bool m_FlipFlop[4];

	uint16 m_NoiseSR;

	uint8 m_CurrentReg;
	uint8 m_CurrentType;
};

