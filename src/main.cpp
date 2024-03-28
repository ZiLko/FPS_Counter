#include <Geode/Geode.hpp>
#include <Geode/Utils.hpp>
#include <chrono>

using namespace geode::prelude;

#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/PlayLayer.hpp>

CCLabelBMFont* counter = nullptr;
Mod* mod = nullptr;

std::chrono::steady_clock::time_point previousUpdate = std::chrono::steady_clock::now();

std::string getFont(int index) {
	if (index <= 1)
		return index == 0 ? "bigFont.fnt" : "chatFont.fnt";
	else if (index < 10) {
		return "gjFont0" + std::to_string(index) + ".fnt";
	}
	else {
		return "gjFont" + std::to_string(index) + ".fnt";
	}
}

class $modify(PlayLayer) {

	bool init(GJGameLevel * level, bool useReplay, bool dontCreateObjects) {
		if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

		if (!mod->getSettingValue<bool>("enabled")) return true;

		counter = CCLabelBMFont::create((std::string("0") + (mod->getSettingValue<bool>("no_text") ? "" : " FPS")).c_str(), getFont(mod->getSettingValue<int64_t>("font") - 1).c_str());
		counter->setScale(static_cast<float>(mod->getSettingValue<double>("size")));
		counter->setOpacity(static_cast<float>(mod->getSettingValue<double>("opacity")) * 255.f);
		counter->setColor(mod->getSettingValue<cocos2d::ccColor3B>("color"));

		auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
		CCPoint pos;
		CCPoint anchor;
		int pI = mod->getSettingValue<int64_t>("position");

		switch (pI) {
		case 1:
			pos = ccp(-winSize.width / 2, winSize.height / 2);
			anchor = ccp(0.f, 1.f);
			break;
		case 2:
			pos = ccp(winSize.width / 2, winSize.height / 2);
			anchor = ccp(1.f, 1.f);
			break;
		case 3:
			pos = ccp(5, 5);
			anchor = ccp(0.f, 0.f);
			break;
		case 4:
			pos = ccp(winSize.width / 2, -winSize.height / 2);
			anchor = ccp(1.f, 0.f);
			break;
		default:
			pos = ccp(-winSize.width / 2, winSize.height / 2);
			anchor = ccp(0.f, 1.f);
			break;
		}

		int multiplierX = (pI == 1 || pI == 3) ? 1 : -1;
		int multiplierY = (pI == 1 || pI == 2) ? -1 : 1;

		counter->setPosition(winSize / 2 + pos + ccp(5.f * multiplierX, 5.f * multiplierY) + ccp(static_cast<float>(mod->getSettingValue<int64_t>("offset_x")), static_cast<float>(mod->getSettingValue<int64_t>("offset_y"))));
		counter->setAnchorPoint(anchor);

		this->addChild(counter);

		return true;
	}
};

class $modify(CCScheduler) {
	void update(float dt) {
		CCScheduler::update(dt);

		if (!mod->getSettingValue<bool>("enabled")) return;

		if (!counter || !PlayLayer::get()) {
			if (counter) {
				counter = nullptr;
			}
			return;
		}

		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - previousUpdate).count() < 500) return;
		previousUpdate = std::chrono::steady_clock::now();

		float fps = 1.f / dt;
		int intFps = static_cast<int>(fps);
		std::string finalFps = (std::to_string(intFps + (fps - intFps > 0.7f ? 1 : 0)) + (mod->getSettingValue<bool>("no_text") ? "" : " FPS"));

		counter->setString(finalFps.c_str());
	}
};

$execute{
	mod = Mod::get();
}