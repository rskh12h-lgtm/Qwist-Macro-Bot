#include "MacroPopup.hpp"
#include "MacroState.hpp"

// Couleurs inspirees d'une fenetre "verre" sombre / magenta
static const ccColor3B kSidebarColor  = {20, 14, 20};
static const ccColor3B kContentColor  = {90, 20, 50};
static const ccColor3B kPillColor     = {60, 90, 60};
static const ccColor3B kFieldColor    = {130, 45, 80};
static const ccColor3B kRedActive     = {235, 90, 90};
static const ccColor3B kGreenActive   = {110, 235, 140};
static const ccColor3B kWhite         = {255, 255, 255};
static const ccColor3B kDim           = {200, 190, 200};

// Popup compact, comme demande
static const float kPopupWidth = 400.f;
static const float kPopupHeight = 270.f;

MacroPopup* MacroPopup::create() {
    auto ret = new MacroPopup();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

CCMenuItemSpriteExtra* MacroPopup::addNavItem(CCMenu* menu, char const* label, float y, bool active, SEL_MenuHandler sel) {
    auto row = CCNode::create();
    row->setContentSize({110.f, 26.f});
    row->setAnchorPoint({0.f, 0.5f});

    if (active) {
        m_activePill = CCScale9Sprite::create("square02_small.png");
        m_activePill->setContentSize({110.f, 26.f});
        m_activePill->setColor(kPillColor);
        m_activePill->setOpacity(140);
        m_activePill->setPosition({55.f, 13.f});
        row->addChild(m_activePill);
    }

    auto dot = CCLabelBMFont::create(active ? "o" : "-", "bigFont.fnt");
    dot->setScale(0.32f);
    dot->setAnchorPoint({0.f, 0.5f});
    dot->setPosition({10.f, 13.f});
    dot->setColor(active ? kWhite : kDim);
    row->addChild(dot);

    auto text = CCLabelBMFont::create(label, "bigFont.fnt");
    text->setScale(0.34f);
    text->setAnchorPoint({0.f, 0.5f});
    text->setPosition({25.f, 13.f});
    text->setColor(active ? kWhite : kDim);
    row->addChild(text);

    auto btn = CCMenuItemSpriteExtra::create(row, this, sel);
    btn->setAnchorPoint({0.f, 0.5f});
    btn->setPosition({12.f, y});
    menu->addChild(btn);
    return btn;
}

bool MacroPopup::init() {
    if (!Popup::init(kPopupWidth, kPopupHeight)) return false;

    this->setTouchEnabled(true);

    float winWidth = kPopupWidth;
    float winHeight = kPopupHeight;
    float sidebarWidth = 115.f;
    m_dragZoneHeight = 26.f; // bande en haut de la fenetre utilisee pour la deplacer

    // --- Sidebar ---
    m_sidebar = CCScale9Sprite::create("square02_small.png");
    m_sidebar->setContentSize({sidebarWidth, winHeight});
    m_sidebar->setColor(kSidebarColor);
    m_sidebar->setOpacity(235);
    m_sidebar->setAnchorPoint({0.f, 0.f});
    m_sidebar->setPosition({0.f, 0.f});
    this->addChild(m_sidebar);

    auto logo = CCLabelBMFont::create("Qwist Macro", "goldFont.fnt");
    logo->setScale(0.36f);
    logo->setAnchorPoint({0.f, 0.5f});
    logo->setPosition({12.f, winHeight - 20.f});
    this->addChild(logo);

    auto navMenu = CCMenu::create();
    navMenu->setPosition({0.f, 0.f});
    navMenu->setAnchorPoint({0.f, 0.f});
    this->addChild(navMenu);

    addNavItem(navMenu, "Record", winHeight - 58.f, true, nullptr);
    addNavItem(navMenu, "Settings", winHeight - 90.f, false, menu_selector(MacroPopup::onSoon));

    // --- Panneau de contenu ---
    float contentX = sidebarWidth + 8.f;
    float contentWidth = winWidth - contentX - 8.f;
    float contentBottom = 8.f;
    float contentTop = winHeight - 8.f;

    m_content = CCScale9Sprite::create("square02_small.png");
    m_content->setContentSize({contentWidth, contentTop - contentBottom});
    m_content->setColor(kContentColor);
    m_content->setOpacity(225);
    m_content->setAnchorPoint({0.f, 0.f});
    m_content->setPosition({contentX, contentBottom});
    this->addChild(m_content);

    float cx = contentX + 18.f;                    // colonne des labels (gauche)
    float valueX = contentX + contentWidth - 18.f;  // colonne des valeurs (droite), alignee pour toutes les lignes
    float top = contentTop - 18.f;
    float rowStep = 26.f;
    float row = 0.f;

    auto rowY = [&](float index) { return top - index * rowStep; };

    // --- Titre + statut ---
    auto title = CCLabelBMFont::create("Record", "goldFont.fnt");
    title->setScale(0.6f);
    title->setAnchorPoint({0.f, 0.5f});
    title->setPosition({cx, rowY(row)});
    this->addChild(title);

    m_statusLabel = CCLabelBMFont::create("Pret", "chatFont.fnt");
    m_statusLabel->setScale(0.38f);
    m_statusLabel->setAnchorPoint({1.f, 0.5f});
    m_statusLabel->setPosition({valueX, rowY(row)});
    this->addChild(m_statusLabel);
    row += 0.9f;

    auto sep1 = CCScale9Sprite::create("square02_small.png");
    sep1->setContentSize({contentWidth - 36.f, 2.f});
    sep1->setColor(kWhite);
    sep1->setOpacity(60);
    sep1->setAnchorPoint({0.f, 0.5f});
    sep1->setPosition({cx, rowY(row)});
    this->addChild(sep1);
    row += 0.85f;

    // --- Replay Name : label a gauche, champ aligne a droite ---
    auto nameLabel = CCLabelBMFont::create("Replay Name", "chatFont.fnt");
    nameLabel->setScale(0.36f);
    nameLabel->setAnchorPoint({0.f, 0.5f});
    nameLabel->setPosition({cx, rowY(row)});
    this->addChild(nameLabel);

    m_nameInput = TextInput::create(130.f, "Replay", "chatFont.fnt");
    m_nameInput->setPosition({valueX - 65.f, rowY(row)});
    m_nameInput->setString(MacroState::get().lastName);
    this->addChild(m_nameInput);
    row += 0.95f;

    // --- Load / Save ---
    auto ioMenu = CCMenu::create();
    ioMenu->setPosition({0.f, 0.f});
    this->addChild(ioMenu);

    float halfW = (contentWidth - 36.f - 10.f) / 2.f;

    auto loadSprite = ButtonSprite::create("Load", "goldFont.fnt", "GJ_button_02.png", 0.65f);
    loadSprite->setContentSize({halfW, 26.f});
    auto loadBtn = CCMenuItemSpriteExtra::create(loadSprite, this, menu_selector(MacroPopup::onLoad));
    loadBtn->setPosition({cx + halfW / 2.f, rowY(row)});
    ioMenu->addChild(loadBtn);

    auto saveSprite = ButtonSprite::create("Save", "goldFont.fnt", "GJ_button_02.png", 0.65f);
    saveSprite->setContentSize({halfW, 26.f});
    auto saveBtn = CCMenuItemSpriteExtra::create(saveSprite, this, menu_selector(MacroPopup::onSave));
    saveBtn->setPosition({cx + halfW + 10.f + halfW / 2.f, rowY(row)});
    ioMenu->addChild(saveBtn);
    row += 0.9f;

    // --- Record / Play (mode) ---
    auto modeMenu = CCMenu::create();
    modeMenu->setPosition({0.f, 0.f});
    this->addChild(modeMenu);

    m_recordSprite = ButtonSprite::create("Record", "goldFont.fnt", "GJ_button_04.png", 0.6f);
    m_recordSprite->setContentSize({halfW, 24.f});
    m_recordBtn = CCMenuItemSpriteExtra::create(m_recordSprite, this, menu_selector(MacroPopup::onRecord));
    m_recordBtn->setPosition({cx + halfW / 2.f, rowY(row)});
    modeMenu->addChild(m_recordBtn);

    m_playSprite = ButtonSprite::create("Play", "goldFont.fnt", "GJ_button_01.png", 0.6f);
    m_playSprite->setContentSize({halfW, 24.f});
    m_playBtn = CCMenuItemSpriteExtra::create(m_playSprite, this, menu_selector(MacroPopup::onPlay));
    m_playBtn->setPosition({cx + halfW + 10.f + halfW / 2.f, rowY(row)});
    modeMenu->addChild(m_playBtn);
    row += 0.85f;

    auto sep2 = CCScale9Sprite::create("square02_small.png");
    sep2->setContentSize({contentWidth - 36.f, 2.f});
    sep2->setColor(kWhite);
    sep2->setOpacity(60);
    sep2->setAnchorPoint({0.f, 0.5f});
    sep2->setPosition({cx, rowY(row)});
    this->addChild(sep2);
    row += 0.85f;

    // --- TPS (informatif) ---
    auto tpsLabel = CCLabelBMFont::create("TPS", "chatFont.fnt");
    tpsLabel->setScale(0.36f);
    tpsLabel->setAnchorPoint({0.f, 0.5f});
    tpsLabel->setPosition({cx, rowY(row)});
    this->addChild(tpsLabel);

    auto tpsPill = CCScale9Sprite::create("square02_small.png");
    tpsPill->setContentSize({60.f, 22.f});
    tpsPill->setColor(kFieldColor);
    tpsPill->setOpacity(220);
    tpsPill->setPosition({valueX - 30.f, rowY(row)});
    this->addChild(tpsPill);

    auto tpsValue = CCLabelBMFont::create(std::to_string((int)MacroState::get().tps).c_str(), "bigFont.fnt");
    tpsValue->setScale(0.36f);
    tpsValue->setPosition(tpsPill->getPosition());
    this->addChild(tpsValue);
    row += 0.85f;

    // --- Actions enregistrees ---
    auto countLabel = CCLabelBMFont::create("Actions", "chatFont.fnt");
    countLabel->setScale(0.36f);
    countLabel->setAnchorPoint({0.f, 0.5f});
    countLabel->setPosition({cx, rowY(row)});
    this->addChild(countLabel);

    auto countPill = CCScale9Sprite::create("square02_small.png");
    countPill->setContentSize({60.f, 22.f});
    countPill->setColor(kFieldColor);
    countPill->setOpacity(220);
    countPill->setPosition({valueX - 30.f, rowY(row)});
    this->addChild(countPill);

    m_countValue = CCLabelBMFont::create("0", "bigFont.fnt");
    m_countValue->setScale(0.36f);
    m_countValue->setPosition(countPill->getPosition());
    this->addChild(m_countValue);
    row += 0.9f;

    // --- Boucle ---
    auto loopLabel = CCLabelBMFont::create("Boucle", "chatFont.fnt");
    loopLabel->setScale(0.36f);
    loopLabel->setAnchorPoint({0.f, 0.5f});
    loopLabel->setPosition({cx, rowY(row)});
    this->addChild(loopLabel);

    auto togglesMenu = CCMenu::create();
    togglesMenu->setPosition({0.f, 0.f});
    this->addChild(togglesMenu);

    m_loopSprite = ButtonSprite::create("OFF", "bigFont.fnt", "GJ_button_02.png", 0.5f);
    m_loopSprite->setContentSize({56.f, 22.f});
    m_loopBtn = CCMenuItemSpriteExtra::create(m_loopSprite, this, menu_selector(MacroPopup::onLoop));
    m_loopBtn->setPosition({valueX - 28.f, rowY(row)});
    togglesMenu->addChild(m_loopBtn);
    row += 0.9f;

    // --- Sauvegarde auto ---
    auto autoSaveLabel = CCLabelBMFont::create("Sauv. auto", "chatFont.fnt");
    autoSaveLabel->setScale(0.36f);
    autoSaveLabel->setAnchorPoint({0.f, 0.5f});
    autoSaveLabel->setPosition({cx, rowY(row)});
    this->addChild(autoSaveLabel);

    m_autoSaveSprite = ButtonSprite::create("ON", "bigFont.fnt", "GJ_button_01.png", 0.5f);
    m_autoSaveSprite->setContentSize({56.f, 22.f});
    m_autoSaveBtn = CCMenuItemSpriteExtra::create(m_autoSaveSprite, this, menu_selector(MacroPopup::onAutoSave));
    m_autoSaveBtn->setPosition({valueX - 28.f, rowY(row)});
    togglesMenu->addChild(m_autoSaveBtn);

    this->refresh();
    return true;
}

void MacroPopup::refresh() {
    auto& state = MacroState::get();

    if (state.recording) {
        m_statusLabel->setString("Enregistrement...");
        m_statusLabel->setColor(kRedActive);
        m_recordSprite->setString("Stop");
        m_recordSprite->setColor(kRedActive);
        m_playSprite->setColor(kWhite);
        m_playSprite->setString("Play");
    } else if (state.playing) {
        m_statusLabel->setString("Lecture...");
        m_statusLabel->setColor(kGreenActive);
        m_playSprite->setString("Stop");
        m_playSprite->setColor(kGreenActive);
        m_recordSprite->setColor(kWhite);
        m_recordSprite->setString("Record");
    } else {
        m_statusLabel->setString("Pret");
        m_statusLabel->setColor(kDim);
        m_recordSprite->setString("Record");
        m_recordSprite->setColor(kWhite);
        m_playSprite->setString("Play");
        m_playSprite->setColor(kWhite);
    }

    m_countValue->setString(std::to_string(state.actions.size()).c_str());

    m_loopSprite->setString(state.loop ? "ON" : "OFF");
    m_loopSprite->setColor(state.loop ? kGreenActive : kWhite);

    m_autoSaveSprite->setString(state.autoSave ? "ON" : "OFF");
    m_autoSaveSprite->setColor(state.autoSave ? kGreenActive : kWhite);
}

void MacroPopup::onRecord(CCObject*) {
    auto& state = MacroState::get();
    state.recording ? state.stopRecording() : state.startRecording();
    this->refresh();
}

void MacroPopup::onPlay(CCObject*) {
    auto& state = MacroState::get();
    state.playing ? state.stopPlayback() : state.startPlayback();
    this->refresh();
}

void MacroPopup::onSave(CCObject*) {
    auto name = m_nameInput->getString();
    if (name.empty()) name = "last";
    MacroState::get().save(name);
    this->refresh();
}

void MacroPopup::onLoad(CCObject*) {
    auto name = m_nameInput->getString();
    if (name.empty()) name = "last";
    MacroState::get().load(name);
    this->refresh();
}

void MacroPopup::onLoop(CCObject*) {
    auto& state = MacroState::get();
    state.loop = !state.loop;
    this->refresh();
}

void MacroPopup::onAutoSave(CCObject*) {
    auto& state = MacroState::get();
    state.autoSave = !state.autoSave;
    this->refresh();
}

void MacroPopup::onSoon(CCObject*) {
    Notification::create("Bientot disponible", NotificationIcon::Info)->show();
}

// --- Deplacement de la fenetre : on ne peut la faire glisser qu'en
// touchant la bande du haut (comme une barre de titre), pour ne jamais
// gener les boutons/champs en dessous.
bool MacroPopup::ccTouchBegan(CCTouch* touch, CCEvent* event) {
    auto loc = this->convertTouchToNodeSpace(touch);
    if (loc.y > kPopupHeight - m_dragZoneHeight && loc.y <= kPopupHeight && loc.x >= 0.f && loc.x <= kPopupWidth) {
        m_dragging = true;
        m_dragTouchStart = touch->getLocation();
        m_dragNodeStart = this->getPosition();
        return true;
    }
    return Popup::ccTouchBegan(touch, event);
}

void MacroPopup::ccTouchMoved(CCTouch* touch, CCEvent* event) {
    if (m_dragging) {
        auto delta = touch->getLocation() - m_dragTouchStart;
        this->setPosition(m_dragNodeStart + delta);
        return;
    }
    Popup::ccTouchMoved(touch, event);
}

void MacroPopup::ccTouchEnded(CCTouch* touch, CCEvent* event) {
    if (m_dragging) {
        m_dragging = false;
        return;
    }
    Popup::ccTouchEnded(touch, event);
}
