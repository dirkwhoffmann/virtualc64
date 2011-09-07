/*
 * (C) 2006 Jérôme Lang. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "C64.h"

const float SIDVoice::decayReleaseTimeRpTable[16] = 
{ (1.0/0.006), (1.0/0.024), (1.0/0.048), (1.0/0.072),
  (1.0/0.114), (1.0/0.168), (1.0/0.204), (1.0/0.240),
  (1.0/0.300), (1.0/0.750), (1.0/1.50) , (1.0/2.40) ,
  (1.0/3.00) , (1.0/9.00) , (1.0/15.0) , (1.0/24.0) };
										   
const float SIDVoice::attackTimeRpTable[16] = 
{ (500.0f), (125.0f), (62.5f), (1.0/0.024), 
  (1.0/0.038), (1.0/0.056), (1.0/0.068), (12.5f),
  (10.0f), (4.0f), (2.0f), (1.25f),
  (1.00f) , (1.0/3.00) , (0.20f) , (0.125f) };
										   
SIDVoice::SIDVoice()
{
	this->samplerate = 44100;
	this->samplerateRp = 1.0 / ((float)this->samplerate);
	this->frequencyFactor = 0.0f;
	this->reset();
}

// reset values to default
void SIDVoice::reset()
{
	this->freq = 0;
	this->calcFreq = 0.0f;
	
	this->pw = 0;
	this->dutyCycle = 0.0;
	
	this->gate = false;
	this->sync = false;
	this->ring = false;
	this->test = false;
	this->wave = WAVE_NONE;
	
	this->filter = false;
	this->mute = false;
	
	this->counter = 0.0f;
	this->addToCounter = 0.0f;
	
	this->randomCount = 0;
	this->lastRandom = 0.0f;
	this->randomReg = 0x7ffff8; //initial value for noise wave
	
	// envelope stuff
	this->setAttackDecay(0x00);
	this->setSustainRelease(0x00);
	this->envelopeValue = 0.0f;
	this->e_state = E_IDLE;
}

void SIDVoice::setAttackDecay(uint8_t value)
{
	// set decay and attack rate
	// equals to "1.0 / (time * samplerate)"
	this->decayRate = decayReleaseTimeRpTable[(value & 0x0F)] * this->samplerateRp; 
	this->attackRate = attackTimeRpTable[((value & 0xF0) >> 4)] * this->samplerateRp;
}


void SIDVoice::setSustainRelease(uint8_t value)
{
	// set sustain level
	float nSustainLevel = OldSID::volumeLevelTable[((value & 0xF0) >> 4)];
	
	// if you try to increase ths sustain level above its current value while the voice is in the
	// sustain phase, the voice will be turned off
	if ((this->e_state == E_SUSTAIN) && (nSustainLevel > sustainLevel))
	{
		this->sustainLevel = 0.0f;
	}
	else
		this->sustainLevel = nSustainLevel;
	
	// set release rate
	this->releaseRate = sustainLevel * decayReleaseTimeRpTable[(value & 0x0F)] * this->samplerateRp; //equal to: "sustainLevel / (time * samplerate)"
}


float SIDVoice::generateEnvelope()
{
	switch (this->e_state) 
	{
		case E_ATTACK:
			envelopeValue += attackRate; // (if attack rate is changed while envelope is in attack phase, the change will have a effect)
			if (envelopeValue >= 1.0f) 
			{
				envelopeValue = 1.0f;
				e_state = E_DECAY;
			}
			break;
		
		case E_DECAY:
			envelopeValue -= decayRate; // if decay rate is changed, while envelope in this phase, change will have a effect
			if (envelopeValue <= sustainLevel) 
			{
				envelopeValue = sustainLevel;
				e_state = E_SUSTAIN;
			}
			break;
		
		case E_RELEASE:
			envelopeValue -= releaseRate; // if release rate is changed, change will have a effect
			if (envelopeValue <= 0.0f)       
			{
				envelopeValue = 0.0f;
				e_state = E_IDLE;
			}
			break;
	}
	return envelopeValue;
}


void SIDVoice::on()
{
	e_state = E_ATTACK;
}


void SIDVoice::off()
{
	e_state = E_RELEASE;
}

