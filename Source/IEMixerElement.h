// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

#include "alsa/asoundlib.h"

enum IEMixerElementCallbackType : uint8_t
{
    None = 1,
    Volume = 1 << 1,
    Mute = 1 << 2,

    Size = 1 << 3,
};

class IEMixerElement
{
public:
    using IEMixerElementCallback = std::function<void(const IEMixerElement&, IEMixerElementCallbackType, void*)>;
    using IEMixerElementCallbackMap = std::unordered_map<uint32_t, std::pair<IEMixerElementCallback, void*>>;

public:
    IEMixerElement(snd_mixer_elem_t* MixerElementHandle = nullptr);
    ~IEMixerElement() = default;

public:
    std::optional<float> GetVolume() const;
    void SetVolume(float Volume);
    std::optional<bool> GetMute() const;
    void SetMute(bool bMute);

    [[nodiscard]] uint32_t RegisterCallback(const IEMixerElementCallback& Func, uint8_t CallbackMask, void* UserData);
    void UnregisterCallback(uint32_t CallbackID);

    void OnMixerEvent();

private:
    IEMixerElementCallbackMap m_RegisteredCallbacks;
    std::unordered_map<uint8_t, std::vector<uint32_t>> m_CallbackTypeLookupMap;
    std::mutex m_MixerElementCallbackMapCriticalSection;

    const std::shared_ptr<snd_mixer_elem_t> m_Handle;
    
    float m_CachedVolume = 0.0f;
    bool m_bCachedMute = false;
};