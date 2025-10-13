PapaPan is a cross-platform auto-panning audio plugin built with JUCE.
It works in VST3, AU, and Standalone formats and lets you move sound smoothly between the left and right channels in several modulation modes.


✨ Features

* Automatic panning with adjustable rate, depth, and waveform.
* Five modulation modes for different sync behaviors.
* Tempo and beat synchronization with the host DAW.
* Real-time modulation visualization.
* Lightweight DSP optimized for real-time use.

---

🧭 Modulation Modes
* **Hertz Retrig**.

Pans the signal at a fixed frequency (Hz).
The phase resets every time playback starts.

* **Beat Retrig.**

Panning speed follows the project tempo (BPM) and a chosen musical duration.
The phase resets on playback start.

* **Hertz Synced**

Like Hertz Retrig., but the phase doesn’t reset — it continues smoothly from the current time, keeping motion continuous.

* **Beat Synced**

Tempo-synchronized panning where phase is recalculated based on the DAW playhead position.
When the play cursor is moved, the panning stays in sync with the beat.

* **Pitch → Rate**

The panning rate depends on the detected pitch (F0) of the input signal — higher notes move faster across the stereo field.

---

🔧 Technical Notes

* Written in C++17 / JUCE 8
* Sample-accurate modulation
* Works in Reaper, Ableton Live, Logic Pro, etc.
* CPU-efficient and designed for real-time processing
