#pragma once
#include <Geode/Geode.hpp>
#include <fstream>

using namespace geode::prelude;

struct MacroAction {
    int frame;
    PlayerButton button;
    bool isPlayer1;
    bool pressed;
};

class MacroState {
public:
    static MacroState& get() {
        static MacroState instance;
        return instance;
    }

    bool recording = false;
    bool playing = false;
    int frame = 0;
    size_t playbackIndex = 0;
    std::vector<MacroAction> actions;
    std::string lastName = "last";
    bool loop = false;
    bool autoSave = true;
    float tps = 240.f;
    float accumulator = 0.f;

    void resetCounters() {
        frame = 0;
        playbackIndex = 0;
        accumulator = 0.f;
    }

    void startRecording() {
        actions.clear();
        resetCounters();
        recording = true;
        playing = false;
        Notification::create("Enregistrement demarre", NotificationIcon::Success)->show();
    }

    void stopRecording() {
        recording = false;

        if (autoSave && !actions.empty()) {
            saveToDisk(lastName);
            Notification::create(
                fmt::format("Enregistrement termine ({} actions, sauvegarde auto)", actions.size()),
                NotificationIcon::Success
            )->show();
        } else {
            Notification::create(
                fmt::format("Enregistrement termine ({} actions)", actions.size()),
                NotificationIcon::Success
            )->show();
        }
    }

    void startPlayback() {
        if (actions.empty()) {
            Notification::create("Aucun macro charge", NotificationIcon::Error)->show();
            return;
        }
        resetCounters();
        playing = true;
        recording = false;
        Notification::create("Lecture demarree", NotificationIcon::Success)->show();
    }

    void stopPlayback() {
        playing = false;
    }

    void record(PlayerButton button, bool isPlayer1, bool pressed) {
        actions.push_back({frame, button, isPlayer1, pressed});
    }

    void save(std::string const& name) {
        if (!saveToDisk(name)) {
            Notification::create("Echec de la sauvegarde", NotificationIcon::Error)->show();
            return;
        }
        Notification::create("Macro sauvegarde", NotificationIcon::Success)->show();
    }

    bool saveToDisk(std::string const& name) {
        auto path = Mod::get()->getSaveDir() / (name + ".macro");
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        size_t count = actions.size();
        file.write(reinterpret_cast<const char*>(&count), sizeof(count));
        for (auto& a : actions) {
            file.write(reinterpret_cast<const char*>(&a), sizeof(MacroAction));
        }
        lastName = name;
        return true;
    }

    bool load(std::string const& name) {
        auto path = Mod::get()->getSaveDir() / (name + ".macro");
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            Notification::create("Echec du chargement", NotificationIcon::Error)->show();
            return false;
        }
        size_t count = 0;
        file.read(reinterpret_cast<char*>(&count), sizeof(count));
        actions.resize(count);
        for (size_t i = 0; i < count; i++) {
            file.read(reinterpret_cast<char*>(&actions[i]), sizeof(MacroAction));
        }
        lastName = name;
        Notification::create(fmt::format("Macro charge ({} actions)", count), NotificationIcon::Success)->show();
        return true;
    }
};
