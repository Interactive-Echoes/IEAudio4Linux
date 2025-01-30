// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>

#include "alsa/asoundlib.h"

class IEMixerElement
{
public:
    using IEMixerElementCallback = std::function<void(void*)>;
    using IEMixerElementCallbackMap = std::unordered_map<uint32_t, std::pair<IEMixerElementCallback, void*>>;

public:
    IEMixerElement(snd_mixer_elem_t* MixerElementHandle = nullptr) :
        m_Handle(MixerElementHandle, [](snd_mixer_elem_t* MixerElementHandle) { if (MixerElementHandle) snd_mixer_elem_free(MixerElementHandle); })
    {}
    ~IEMixerElement() = default;

public:
    std::optional<float> GetVolume() const;
    void SetVolume(float Volume) const;
    std::optional<bool> GetMute() const;
    void SetMute(bool bMute) const;

    const IEMixerElementCallbackMap& GetRegisteredCallbacks() const;
    [[nodiscard]] uint32_t RegisterCallback(const std::function<void(void*)>& Func, void* UserData);
    void UnregisterCallback(uint32_t CallbackID);

private:
    const std::shared_ptr<snd_mixer_elem_t> m_Handle;
    IEMixerElementCallbackMap m_RegisteredCallbacks;
};