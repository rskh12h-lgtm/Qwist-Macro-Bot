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

static const float kPopupWidth = 420.f;
static const float kPopupHeight = 260.f;

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
    row->setContentSize({120.f, 30.f});
    row->setAnchorPoint({0.f, 0.5f});

    if (active) {
        m_activePill = CCScale9Sprite::create("square02_small.png");
        m_activePill->setContentSize({120.f, 30.f});
        m_activePill->setColor(kPillColor);
        m_activePill->setOpacity(140);
        m_activePill->setPosition({60.f, 15.f});
        row->addChild(m_activePill);
    }

    auto dot = CCLabelBMFont::create(active ? "o" : "-", "bigFont.fnt");
    dot->setScale(0.35f);
    dot->setAnchorPoint({0.f, 0.5f});
    dot->setPosition({12.f, 15.f});
    dot->setColor(active ? kWhite : kDim);
    row->addChild(dot);

    auto text = CCLabelBMFont::create(label, "bigFont.fnt");
    text->setScale(0.38f);
    text->setAnchorPoint({0.f, 0.5f});
    text->setPosition({28.f, 15.f});
    text->setColor(active ? kWhite : kDim);
    row->addChild(text);

    auto btn = CCMenuItemSpriteExtra::create(row, this, sel);
    btn->setAnchorPoint({0.f, 0.5f});
    btn->setPosition({15.f, y});
    menu->addChild(btn);
    return btn;
}

bool MacroPopup::init() {
    if (!Popup::init(kPopupWidth, kPopupHeight)) return false;

    float winWidth = kPopupWidth;
    float winHeight = kPopupHeight;
    float sidebarWidth = 130.f;

    // --- Sidebar ---
    m_sidebar = CCScale9Sprite::create("square02_small.png");
    m_sidebar->setContentSize({sidebarWidth, winHeight});
    m_sidebar->setColor(kSidebarColor);
    m_sidebar->setOpacity(235);
    m_sidebar->setAnchorPoint({0.f, 0.f});
    m_sidebar->setPosition({0.f, 0.f});
    this->addChild(m_sidebar);

    auto logo = CCLabelBMFont::create("Qwist Macro", "goldFont.fnt");
    logo->setScale(0.4f);
    logo->setAnchorPoint({0.f, 0.5f});
    logo->setPosition({16.f, winHeight - 26.f});
    this->addChild(logo);

    auto navMenu = CCMenu::create();
    navMenu->setPosition({0.f, 0.f});
    navMenu->setAnchorPoint({0.f, 0.f});
    this->addChild(navMenu);

    addNavItem(navMenu, "Macro", winHeight - 70.f, true, nullptr);
    addNavItem(navMenu, "Reglages", winHeight - 106.f, false, menu_selector(MacroPopup::onSoon));

    // --- Panneau de contenu ---
    float contentX = sidebarWidth + 10.f;
    float contentWidth = winWidth - contentX - 10.f;

    m_content = CCScale9Sprite::create("square02_small.png");
    m_content->setContentSize({contentWidth, winHeight - 20.f});
    m_content->setColor(kContentColor);
    m_content->setOpacity(225);
    m_content->setAnchorPoint({0.f, 0.f});
    m_content->setPosition({contentX, 10.f});
    this->addChild(m_content);

    float cx = contentX + 22.f;
    float top = winHeight - 34.f;

    auto title = CCLabelBMFont::create("Macro", "goldFont.fnt");
    title->setScale(0.75f);
    title->setAnchorPoint({0.f, 0.5f});
    title->setPosition({cx, top});
    this->addChild(title);

    m_statusLabel = CCLabelBMFont::create("Pret", "chatFont.fnt");
    m_statusLabel->setScale(0.45f);
    m_statusLabel->setAnchorPoint({1.f, 0.5f});
    m_statusLabel->setPosition({contentX + contentWidth - 20.f, top});
    this->addChild(m_statusLabel);

    auto sep1 = CCScale9Sprite::create("square02_small.png");
    sep1->setContentSize({contentWidth - 40.f, 2.f});
    sep1->setColor(kWhite);
    sep1->setOpacity(60);
    sep1->setAnchorPoint({0.f, 0.5f});
    sep1->setPosition({cx, top - 16.f});
    this->addChild(sep1);

    // --- Nom du macro ---
    auto nameLabel = CCLabelBMFont::create("Nom du macro", "chatFont.fnt");
    nameLabel->setScale(0.45f);
    nameLabel->setAnchorPoint({0.f, 0.5f});
    nameLabel->setPosition({cx, top - 40.f});
    this->addChild(nameLabel);

    m_nameInput = TextInput::create(150.f, "Replay", "chatFont.fnt");
    m_nameInput->setAnchorPoint({1.f, 0.5f});
    m_nameInput->setPosition({contentX + contentWidth - 20.f, top - 40.f});
    m_nameInput->setString(MacroState::get().lastName);
    this->addChild(m_nameInput);

    // --- Charger / Sauvegarder ---
    auto ioMenu = CCMenu::create();
    ioMenu->setPosition({0.f, 0.f});
    this->addChild(ioMenu);

    float rowY = top - 70.f;
    float halfW = (contentWidth - 44.f) / 2.f;

    auto loadSprite = ButtonSprite::create("Charger", "goldFont.fnt", "GJ_button_02.png", 0.7f);
    loadSprite->setContentSize({halfW, 30.f});
    auto loadBtn = CCMenuItemSpriteExtra::create(loadSprite, this, menu_selector(MacroPopup::onLoad));
    loadBtn->setPosition({cx + halfW / 2.f, rowY});
    ioMenu->addChild(loadBtn);

    auto saveSprite = ButtonSprite::create("Sauvegarder", "goldFont.fnt", "GJ_button_02.png", 0.7f);
    saveSprite->setContentSize({halfW, 30.f});
    auto saveBtn = CCMenuItemSpriteExtra::create(saveSprite, this, menu_selector(MacroPopup::onSave));
    saveBtn->setPosition({cx + halfW + 14.f + halfW / 2.f, rowY});
    ioMenu->addChild(saveBtn);

    // --- Enregistrer / Lire (mode) ---
    auto modeMenu = CCMenu::create();
    modeMenu->setPosition({0.f, 0.f});
    this->addChild(modeMenu);

    float rowY2 = rowY - 38.f;

    m_recordSprite = ButtonSprite::create("Enregistrer", "goldFont.fnt", "GJ_button_04.png", 0.7f);
    m_recordSprite->setContentSize({halfW, 30.f});
    m_recordBtn = CCMenuItemSpriteExtra::create(m_recordSprite, this, menu_selector(MacroPopup::onRecord));
    m_recordBtn->setPosition({cx + halfW / 2.f, rowY2});
    modeMenu->addChild(m_recordBtn);

    m_playSprite = ButtonSprite::create("Lire", "goldFont.fnt", "GJ_button_01.png", 0.7f);
    m_playSprite->setContentSize({halfW, 30.f});
    m_playBtn = CCMenuItemSpriteExtra::create(m_playSprite, this, menu_selector(MacroPopup::onPlay));
    m_playBtn->setPosition({cx + halfW + 14.f + halfW / 2.f, rowY2});
    modeMenu->addChild(m_playBtn);

    // --- Separateur ---
    auto sep2 = CCScale9Sprite::create("square02_small.png");
    sep2->setContentSize({contentWidth - 40.f, 2.f});
    sep2->setColor(kWhite);
    sep2->setOpacity(60);
    sep2->setAnchorPoint({0.f, 0.5f});
    sep2->setPosition({cx, rowY2 - 24.f});
    this->addChild(sep2);

    // --- Actions enregistrees ---
    float rowY3 = rowY2 - 46.f;

    auto countLabel = CCLabelBMFont::create("Actions enregistrees", "chatFont.fnt");
    countLabel->setScale(0.42f);
    countLabel->setAnchorPoint({0.f, 0.5f});
    countLabel->setPosition({cx, rowY3});
    this->addChild(countLabel);

    auto countPill = CCScale9Sprite::create("square02_small.png");
    countPill->setContentSize({60.f, 24.f});
    countPill->setColor(kFieldColor);
    countPill->setOpacity(220);
    countPill->setAnchorPoint({1.f, 0.5f});
    countPill->setPosition({contentX + contentWidth - 20.f, rowY3});
    this->addChild(countPill);

    m_countValue = CCLabelBMFont::create("0", "bigFont.fnt");
    m_countValue->setScale(0.4f);
    m_countValue->setPosition(countPill->getPosition() - CCPoint{30.f, 0.f});
    this->addChild(m_countValue);

    // --- Boucle ---
    float rowY4 = rowY3 - 34.f;

    auto loopLabel = CCLabelBMFont::create("Boucle", "chatFont.fnt");
    loopLabel->setScale(0.42f);
    loopLabel->setAnchorPoint({0.f, 0.5f});
    loopLabel->setPosition({cx, rowY4});
    this->addChild(loopLabel);

    auto loopMenu = CCMenu::create();
    loopMenu->setPosition({0.f, 0.f});
    this->addChild(loopMenu);

    m_loopSprite = ButtonSprite::create("OFF", "bigFont.fnt", "GJ_button_02.png", 0.6f);
    m_loopSprite->setContentSize({60.f, 24.f});
    m_loopBtn = CCMenuItemSpriteExtra::create(m_loopSprite, this, menu_selector(MacroPopup::onLoop));
    m_loopBtn->setPosition({contentX + contentWidth - 20.f - 30.f, rowY4});
    loopMenu->addChild(m_loopBtn);

    this->refresh();
    return true;
}

void MacroPopup::refresh() {
    auto& state = MacroState::get();

    if (state.recording) {
        m_statusLabel->setString("Enregistrement...");
        m_statusLabel->setColor(kRedActive);
        m_recordSprite->setString("Arreter");
        m_recordSprite->setColor(kRedActive);
        m_playSprite->setColor(kWhite);
        m_playSprite->setString("Lire");
    } else if (state.playing) {
        m_statusLabel->setString("Lecture...");
        m_statusLabel->setColor(kGreenActive);
        m_playSprite->setString("Arreter");
        m_playSprite->setColor(kGreenActive);
        m_recordSprite->setColor(kWhite);
        m_recordSprite->setString("Enregistrer");
    } else {
        m_statusLabel->setString("Pret");
        m_statusLabel->setColor(kDim);
        m_recordSprite->setString("Enregistrer");
        m_recordSprite->setColor(kWhite);
        m_playSprite->setString("Lire");
        m_playSprite->setColor(kWhite);
    }

    m_countValue->setString(std::to_string(state.actions.size()).c_str());

    m_loopSprite->setString(state.loop ? "ON" : "OFF");
    m_loopSprite->setColor(state.loop ? kGreenActive : kWhite);
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

void MacroPopup::onSoon(CCObject*) {
    Notification::create("Bientot disponible", NotificationIcon::Info)->show();
}
