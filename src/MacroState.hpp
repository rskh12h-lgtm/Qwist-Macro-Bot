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

    void resetCounters() {
        frame = 0;
        playbackIndex = 0;
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
        Notification::create(
            fmt::format("Enregistrement termine ({} actions)", actions.size()),
            NotificationIcon::Success
        )->show();
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
        auto path = Mod::get()->getSaveDir() / (name + ".macro");
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) {
            Notification::create("Echec de la sauvegarde", NotificationIcon::Error)->show();
            return;
        }
        size_t count = actions.size();
        file.write(reinterpret_cast<const char*>(&count), sizeof(count));
        for (auto& a : actions) {
            file.write(reinterpret_cast<const char*>(&a), sizeof(MacroAction));
        }
        lastName = name;
        Notification::create("Macro sauvegarde", NotificationIcon::Success)->show();
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
