#include "lolgame.h"

LoLGame::LoLGame(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

LoLGame::LoLGame(LoLData::Game* game, QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	if (game->win) {
		this->setStyleSheet("background:rgb(150, 217, 234);");
	}
	else {
		this->setStyleSheet("background:rgb(224, 192, 206);");
	}
	

	auto it = LoLData::GAME_TYPE_TO_DISPLAY.find(game->gameType);
	if (it != LoLData::GAME_TYPE_TO_DISPLAY.end())	// Game type found
		ui.gameTypeLabel->setText(LoLData::GAME_TYPE_TO_DISPLAY[game->gameType]);
	else {
		ui.gameTypeLabel->setText(game->gameType);
	}

	ui.KDALabel->setText("K/D/A: " + QString::number(game->kills) + "/" + 
		QString::number(game->deaths) + "/" + QString::number(game->assists));
	ui.timeLabel->setText(game->gameDate.toString("MM/dd/yyyy"));
	for (int i = 0; i < 5; i++) {
		LoLData::GamePlayer player = game->team1Players[i];
	}
	for (int i = 0; i < 5; i++) {
		LoLData::GamePlayer player = game->team2Players[i];
	}
	setChampionIcon(ui.championLabel, game->championID);
	setChampionIcon(ui.player1ChampionLabel, game->team1Players[0].championID);
	setChampionIcon(ui.player2ChampionLabel, game->team1Players[1].championID);
	setChampionIcon(ui.player3ChampionLabel, game->team1Players[2].championID);
	setChampionIcon(ui.player4ChampionLabel, game->team1Players[3].championID);
	setChampionIcon(ui.player5ChampionLabel, game->team1Players[4].championID);
	setChampionIcon(ui.player6ChampionLabel, game->team2Players[0].championID);
	setChampionIcon(ui.player7ChampionLabel, game->team2Players[1].championID);
	setChampionIcon(ui.player8ChampionLabel, game->team2Players[2].championID);
	setChampionIcon(ui.player9ChampionLabel, game->team2Players[3].championID);
	setChampionIcon(ui.player10ChampionLabel, game->team2Players[4].championID);
	setSummonerSpellIcon(ui.spell1Label, game->spell1ID);
	setSummonerSpellIcon(ui.spell2Label, game->spell2ID);
	setItemIcon(ui.item1Label, game->item1ID);
	setItemIcon(ui.item2Label, game->item2ID);
	setItemIcon(ui.item3Label, game->item3ID);
	setItemIcon(ui.item4Label, game->item4ID);
	setItemIcon(ui.item5Label, game->item5ID);
	setItemIcon(ui.item6Label, game->item6ID);
	setPlayerName(ui.player1NameLabel, game->team1Players[0].playerID);
	setPlayerName(ui.player2NameLabel, game->team1Players[1].playerID);
	setPlayerName(ui.player3NameLabel, game->team1Players[2].playerID);
	setPlayerName(ui.player4NameLabel, game->team1Players[3].playerID);
	setPlayerName(ui.player5NameLabel, game->team1Players[4].playerID);
	setPlayerName(ui.player6NameLabel, game->team1Players[5].playerID);
	setPlayerName(ui.player7NameLabel, game->team1Players[6].playerID);
	setPlayerName(ui.player8NameLabel, game->team1Players[7].playerID);
	setPlayerName(ui.player9NameLabel, game->team1Players[8].playerID);
	setPlayerName(ui.player10NameLabel, game->team1Players[9].playerID);
}

LoLGame::~LoLGame()
{

}

void LoLGame::setChampionIcon(QLabel* label, int championID) {
	QString imageName = LoLData::ID_TO_CHAMPION[championID].imageName;
	label->setPixmap(QPixmap("Resources/Champion Icons/" + imageName));
}

void LoLGame::setSummonerSpellIcon(QLabel* label, int summonerSpellIcon) {
	QString imageName = LoLData::ID_TO_SUMMONER[summonerSpellIcon].imageName;
	label->setPixmap(QPixmap("Resources/Summoner Spell Icons/" + imageName));
}

void LoLGame::setItemIcon(QLabel* label, int itemID) {
	QString imageName = LoLData::ID_TO_ITEM[itemID].imageName;
	label->setPixmap(QPixmap("Resources/Item Icons/" + imageName));
}

void LoLGame::setPlayerName(QLabel* label, int playerID) {
	QString name = LoLData::ID_TO_PLAYER[playerID];
	label->setText(name);
}