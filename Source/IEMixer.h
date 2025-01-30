// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include <memory>
#include <string>

#include "alsa/asoundlib.h"

#include "IEMixerElement.h"

class IEMixer
{
public:
    using IEMixerElementMap = std::unordered_map<std::string, IEMixerElement>;

public:
    IEMixer(snd_mixer_t* MixerHandle = nullptr) :
        m_Handle(MixerHandle, [](snd_mixer_t* MixerHandle) { if (MixerHandle) snd_mixer_close(MixerHandle); })
    {}
    ~IEMixer() = default;

public:
    IEMixerElement& GetElement(const char* ElementName);
    void PollingFunction() const;

private:
    const std::shared_ptr<snd_mixer_t> m_Handle;
    IEMixerElementMap m_MixerElementMap;
};