// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include <format>
#include <stdexcept>
#include <string>
#include <thread>

#include "alsa/asoundlib.h"

#include "IEMixerElement.h"

class IEMixer
{
public:
    using IEMixerElementMap = std::unordered_map<std::string, IEMixerElement>;

public:
    IEMixer(snd_mixer_t* MixerHandle = nullptr);
    ~IEMixer();

public:
    IEMixerElement& GetElement(const char* ElementName);

private:
    void PollingFunction();

private:
    IEMixerElementMap m_MixerElementMap;
    std::mutex m_MixerElementMapCriticalSection;

    const std::shared_ptr<snd_mixer_t> m_Handle;
    
    std::thread m_PollingThread;
};