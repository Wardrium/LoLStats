#include <QUrl>
#include <QDebug>
#include <QFileInfo>
#include <QChart>
#include <QLineSeries>
#include <QChartView>
#include <QValueAxis>

#include "lolstats.h"
#include "lolgame.h"

using QtCharts::QChart;
using QtCharts::QLineSeries;
using QtCharts::QChartView;
using QtCharts::QValueAxis;

const QString API_KEY = "RGAPI-b684def8-bd9c-4e15-bdc0-44017a9ab922";
QHash<int, LoLStats::Champion> LoLStats::ID_TO_CHAMPION;
QHash<int, LoLStats::Item> LoLStats::ID_TO_ITEM;
QHash<int, LoLStats::SummonerSpell> LoLStats::ID_TO_SUMMONER;
QHash<int, QString> LoLStats::ID_TO_PLAYER;

// Map from Riot API string to Rank
const QHash<QString, LoLStats::Rank> STRING_TO_RANK({ { "BRONZEV", LoLStats::BRONZE_5 }, { "BRONZEIV", LoLStats::BRONZE_4 },
	{ "BRONZEIII", LoLStats::BRONZE_3 },{ "BRONZEII", LoLStats::BRONZE_2 },{ "BRONZEI", LoLStats::BRONZE_1 },
	{ "SILVERV", LoLStats::SILVER_5 },{ "SILVERIV", LoLStats::SILVER_4 },{ "SILVERIII", LoLStats::SILVER_3 },
	{ "SILVERII", LoLStats::SILVER_2 },{ "SILVERI", LoLStats::SILVER_1 },{ "GOLDV", LoLStats::GOLD_5 },
	{ "GOLDIV", LoLStats::GOLD_4 },{ "GOLDIII", LoLStats::GOLD_3 },{ "GOLDII", LoLStats::GOLD_2 },
	{ "GOLDI", LoLStats::GOLD_1 },{ "PLATINUMV", LoLStats::PLATINUM_5 },{ "PLATINUMIV", LoLStats::PLATINUM_4 },
	{ "PLATINUMIII", LoLStats::PLATINUM_3 },{ "PLATINUMII", LoLStats::PLATINUM_2 },{ "PLATINUMI", LoLStats::PLATINUM_1 },
	{ "DIAMONDV", LoLStats::DIAMOND_5 },{ "DIAMONDIV", LoLStats::DIAMOND_4 },{ "DIAMONDIII", LoLStats::DIAMOND_3 },
	{ "DIAMONDII", LoLStats::DIAMOND_2 },{ "DIAMONDI", LoLStats::DIAMOND_1 },{ "MASTERI", LoLStats::MASTER },
	{ "CHALLENGERI", LoLStats::CHALLENGER } });
// Map from Rank to display string
const QHash<LoLStats::Rank, QString> RANK_TO_DISPLAY({ { LoLStats::BRONZE_5, "Bronze 5" },{ LoLStats::BRONZE_4, "Bronze 4" },
	{ LoLStats::BRONZE_3, "Bronze 3" },{ LoLStats::BRONZE_2, "Bronze 2" },{ LoLStats::BRONZE_1, "Bronze 1" },
	{ LoLStats::SILVER_5, "Silver 5" },{ LoLStats::SILVER_4, "Silver 4" },{ LoLStats::SILVER_3, "Silver 3" },
	{ LoLStats::SILVER_2, "Silver 2" },{ LoLStats::SILVER_1, "Silver 1" },{ LoLStats::GOLD_5, "Gold 5" },
	{ LoLStats::GOLD_4, "Gold 4" },{ LoLStats::GOLD_3, "Gold 3" },{ LoLStats::GOLD_2, "Gold 2" },
	{ LoLStats::GOLD_1, "Gold 1" },{ LoLStats::PLATINUM_5, "Platinum 5" },{ LoLStats::PLATINUM_4, "Platinum 4" },
	{ LoLStats::PLATINUM_3, "Platinum 3" },{ LoLStats::PLATINUM_2, "Platinum 2" },{ LoLStats::PLATINUM_1, "Platinum 1" },
	{ LoLStats::DIAMOND_5, "Diamond 5" },{ LoLStats::DIAMOND_4, "Diamond 4" },{ LoLStats::DIAMOND_3, "Diamond 3" },
	{ LoLStats::DIAMOND_2, "Diamond 2" },{ LoLStats::DIAMOND_1, "Diamond 1" },{ LoLStats::MASTER, "Master" },
	{ LoLStats::CHALLENGER, "Challenger" },{ LoLStats::UNRANKED, "Unranked" } });

// Map from Rank to icon png name
const QHash<LoLStats::Rank, QString> RANK_TO_ICON({ { LoLStats::BRONZE_5, "bronze_5.png" },{ LoLStats::BRONZE_4, "bronze_4.png" },
	{ LoLStats::BRONZE_3, "bronze_3.png" },{ LoLStats::BRONZE_2, "bronze_2.png" },{ LoLStats::BRONZE_1, "bronze_1.png" },
	{ LoLStats::SILVER_5, "silver_5.png" },{ LoLStats::SILVER_4, "silver_4.png" },{ LoLStats::SILVER_3, "silver_3.png" },
	{ LoLStats::SILVER_2, "silver_2.png" },{ LoLStats::SILVER_1, "silver_1.png" },{ LoLStats::GOLD_5, "gold_5.png" },
	{ LoLStats::GOLD_4, "gold_4.png" },{ LoLStats::GOLD_3, "gold_3.png" },{ LoLStats::GOLD_2, "gold_2.png" },
	{ LoLStats::GOLD_1, "gold_1.png" },{ LoLStats::PLATINUM_5, "platinum_5.png" },{ LoLStats::PLATINUM_4, "platinum_4.png" },
	{ LoLStats::PLATINUM_3, "platinum_3.png" },{ LoLStats::PLATINUM_2, "platinum_2.png" },{ LoLStats::PLATINUM_1, "platinum_1.png" },
	{ LoLStats::DIAMOND_5, "diamond_5.png" },{ LoLStats::DIAMOND_4, "diamond_4.png" },{ LoLStats::DIAMOND_3, "diamond_3.png" },
	{ LoLStats::DIAMOND_2, "diamond_2.png" },{ LoLStats::DIAMOND_1, "diamond_1.png" },{ LoLStats::MASTER, "master.png" },
	{ LoLStats::CHALLENGER, "challenger.png" },{ LoLStats::UNRANKED, "unranked.png" } });

// Map from game subType string to display string.
const QHash<QString, QString> GAME_TYPE_TO_DISPLAY ({ { "RANKED_SOLO_5x5", "Ranked Solo" },
	{ "RANKED_FLEX_SR", "Ranked Flex" }, {"NORMAL", "Normal"}, {"ARAM_UNRANKED", "ARAM"},
	{ "RANKED_TEAM_3x3", "Ranked TT"}, {"NORMAL_3x3", "Twisted Treeline"} });

LoLStats::LoLStats(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.searchButton, SIGNAL(clicked()), this, SLOT(searchPlayer()));
	connect(ui.newSearchButton, SIGNAL(clicked()), this, SLOT(switchSearchPage()));
	connect(ui.debugButton, SIGNAL(clicked()), this, SLOT(debug()));
	queryChampionData(&LoLStats::loadChampionData);
	querySummonerSpellData(&LoLStats::loadSummonerSpellData);
	queryItemData(&LoLStats::loadItemData);
}

LoLStats::~LoLStats()
{

}

void LoLStats::searchPlayer() {
	// Reset values
	ID = -1;
	summonerLevel = 0;
	soloRank = UNRANKED;
	soloLP = 0;
	soloWins = 0;
	soloLosses = 0;
	flexRank = UNRANKED;
	flexLP = 0;
	flexWins = 0;
	flexLosses = 0;
	championIDs.clear();
	summonerSpellIDs.clear();
	itemIDs.clear();
	playerIDs.clear();
	clearHistory();
	games.clear();

	username = ui.usernameText->text();
	querySummonerByName(username, &LoLStats::loadPlayerData);
}

void LoLStats::prepareChampionIcons() {
	numChampionIcons = 0;
	for (auto it = championIDs.begin(); it != championIDs.end(); it++) {
		if (QFileInfo::exists("Resources/Champion Icons/" + ID_TO_CHAMPION[*it].imageName)) {
			loadChampionIcons();
		}
		else {
			queryChampionIcon(*it, &LoLStats::downloadChampionIcon);
		}
	}
}

void LoLStats::prepareSummonerSpellIcons() {
	numSummonerSpellIcons = 0;
	for (auto it = summonerSpellIDs.begin(); it != summonerSpellIDs.end(); it++) {
		if (QFileInfo::exists("Resources/Summoner Spell Icons/" + ID_TO_SUMMONER[*it].imageName)) {
			loadSummonerSpellIcons();
		}
		else {
			querySummonerSpellIcon(*it, &LoLStats::downloadSummonerSpellIcon);
		}
	}
}

void LoLStats::prepareItemIcons() {
	numItemIcons = 0;
	for (auto it = itemIDs.begin(); it != itemIDs.end(); it++) {
		if (QFileInfo::exists("Resources/Item Icons/" + ID_TO_ITEM[*it].imageName)) {
			loadItemIcons();
		}
		else {
			queryItemIcon(*it, &LoLStats::downloadItemIcon);
		}
	}
}

void LoLStats::preparePlayerNames() {
	QString queryIDs = "";
	for (auto it = playerIDs.begin(); it != playerIDs.end(); it++) {
		queryIDs += QString::number(*it) + ",";
	}
	queryIDs.remove(queryIDs.length() - 1);	// Remove last comma
	querySummonerByIDs(queryIDs, &LoLStats::loadPlayerNames);
}

// Functions for querying Riot API >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
void LoLStats::queryChampionData(void (LoLStats::*callback)()) {
	QUrl url("http://ddragon.leagueoflegends.com/cdn/7.2.1/data/en_US/champion.json");
	querySendRequest(url, callback);
}

void LoLStats::querySummonerSpellData(void (LoLStats::*callback)()) {
	QUrl url("http://ddragon.leagueoflegends.com/cdn/7.2.1/data/en_US/summoner.json");
	querySendRequest(url, callback);
}

void LoLStats::queryItemData(void (LoLStats::*callback)()) {
	QUrl url("http://ddragon.leagueoflegends.com/cdn/7.2.1/data/en_US/item.json");
	querySendRequest(url, callback);
}

void LoLStats::queryChampionIcon(int ID, void (LoLStats::*callback)()) {
	QUrl url("http://ddragon.leagueoflegends.com/cdn/7.2.1/img/champion/" + ID_TO_CHAMPION[ID].imageName);
	querySendRequest(url, callback);
}

void LoLStats::querySummonerSpellIcon(int ID, void (LoLStats::*callback)()) {
	QUrl url("http://ddragon.leagueoflegends.com/cdn/7.2.1/img/spell/" + ID_TO_SUMMONER[ID].imageName);
	querySendRequest(url, callback);
}

void LoLStats::queryItemIcon(int ID, void (LoLStats::*callback)()) {
	QUrl url("http://ddragon.leagueoflegends.com/cdn/7.2.1/img/item/" + ID_TO_ITEM[ID].imageName);
	querySendRequest(url, callback);
}

void LoLStats::querySummonerByName(const QString& username, void (LoLStats::*callback)()) {
	QUrl url("https://na.api.pvp.net/api/lol/na/v1.4/summoner/by-name/" + username + "?api_key=" + API_KEY);
	querySendRequest(url, callback);
}

void LoLStats::querySummonerByIDs(const QString& IDs, void (LoLStats::*callback)()) {
	QUrl url("https://na.api.pvp.net/api/lol/na/v1.4/summoner/" + IDs + "?api_key=" + API_KEY);
	querySendRequest(url, callback);
}

void LoLStats::queryLeagueBySummonerID(int ID, void (LoLStats::*callback)()) {
	QUrl url("https://na.api.pvp.net/api/lol/na/v2.5/league/by-summoner/" + QString::number(ID) + "?api_key=" + API_KEY);
	querySendRequest(url, callback);
}

void LoLStats::queryGameBySummonerID(int ID, void(LoLStats::*callback)()) {
	QUrl url("https://na.api.pvp.net/api/lol/na/v1.3/game/by-summoner/" + QString::number(ID) + "/recent?api_key=" + API_KEY);
	querySendRequest(url, callback);
}

void LoLStats::querySendRequest(QUrl url, void(LoLStats::*callback)()) {
	QNetworkReply* reply = qnam.get(QNetworkRequest(url));	// Makes the get request
	connect(reply, &QNetworkReply::finished, this, callback);
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// Callback functions for loading data after querying Riot API >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
void LoLStats::loadChampionData() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());
	if (reply->error() == QNetworkReply::NoError) {
		QString json_res(reply->readAll());
		QJsonObject res = QJsonDocument::fromJson(json_res.toUtf8()).object();
		QJsonObject data = res["data"].toObject();

		for (auto it = data.begin(); it != data.end(); it++) {
			QJsonObject championData = it->toObject();
			int ID = championData["key"].toString().toInt();
			QString name = championData["name"].toString();
			QString imageName = championData["image"].toObject()["full"].toString();
			ID_TO_CHAMPION[ID] = Champion(name, imageName);
		}
	}
	reply->deleteLater();
}

void LoLStats::loadSummonerSpellData() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());
	if (reply->error() == QNetworkReply::NoError) {
		QString json_res(reply->readAll());
		QJsonObject res = QJsonDocument::fromJson(json_res.toUtf8()).object();
		QJsonObject data = res["data"].toObject();

		for (auto it = data.begin(); it != data.end(); it++) {
			QJsonObject summonerData = it->toObject();
			int ID = summonerData["key"].toString().toInt();
			QString name = summonerData["name"].toString();
			QString imageName = summonerData["image"].toObject()["full"].toString();
			ID_TO_SUMMONER[ID] = SummonerSpell(name, imageName);
		}
	}
	reply->deleteLater();
}

void LoLStats::loadItemData() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());
	if (reply->error() == QNetworkReply::NoError) {
		QString json_res(reply->readAll());
		QJsonObject res = QJsonDocument::fromJson(json_res.toUtf8()).object();
		QJsonObject data = res["data"].toObject();

		for (auto it = data.begin(); it != data.end(); it++) {
			QJsonObject itemData = it->toObject();
			int ID = it.key().toInt();
			QString name = itemData["name"].toString();
			QString imageName = itemData["image"].toObject()["full"].toString();
			ID_TO_ITEM[ID] = Item(name, imageName);
		}
	}
	reply->deleteLater();
}

void LoLStats::downloadChampionIcon() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());
	if (reply->error() == QNetworkReply::NoError) {
		QByteArray imageData = reply->readAll();
		saveDataToFile(imageData, "Resources/Champion Icons/" + reply->url().fileName());
	}
	reply->deleteLater();
	loadChampionIcons();
}

void LoLStats::downloadSummonerSpellIcon() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());
	if (reply->error() == QNetworkReply::NoError) {
		QByteArray imageData = reply->readAll();
		saveDataToFile(imageData, "Resources/Summoner Spell Icons/" + reply->url().fileName());
	}
	reply->deleteLater();
	loadSummonerSpellIcons();
}

void LoLStats::downloadItemIcon() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());
	if (reply->error() == QNetworkReply::NoError) {
		QByteArray imageData = reply->readAll();
		saveDataToFile(imageData, "Resources/Item Icons/" + reply->url().fileName());
	}
	reply->deleteLater();
	loadItemIcons();
}

void LoLStats::loadChampionIcons() {
	numChampionIcons += 1;
	if (numChampionIcons == championIDs.size()) {
		alertPlayerDataQueryFinished();
	}
}

void LoLStats::loadSummonerSpellIcons() {
	numSummonerSpellIcons += 1;
	if (numSummonerSpellIcons == summonerSpellIDs.size()) {
		alertPlayerDataQueryFinished();
	}
}

void LoLStats::loadItemIcons() {
	numItemIcons += 1;
	if (numItemIcons == itemIDs.size()) {
		alertPlayerDataQueryFinished();
	}
}

void LoLStats::loadPlayerNames() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());
	if (reply->error() == QNetworkReply::NoError) {
		QString json_res(reply->readAll());
		QJsonObject data = QJsonDocument::fromJson(json_res.toUtf8()).object();
		ID_TO_PLAYER[ID] = username;
		for (auto it = data.begin(); it != data.end(); it++) {
			int ID = it.key().toInt();
			QString name = it->toObject()["name"].toString();
			ID_TO_PLAYER[ID] = name;
		}
	}
	alertPlayerDataQueryFinished();
	reply->deleteLater();
}

void LoLStats::loadPlayerData() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());
	if (reply->error() == QNetworkReply::NoError) {
		QString json_res(reply->readAll());
		QJsonObject res = QJsonDocument::fromJson(json_res.toUtf8()).object();
		QJsonObject data = res[username].toObject();

		username = data["name"].toString();		// Redefine username to fix capitilization
		ID = data["id"].toInt();
		summonerLevel = data["summonerLevel"].toInt();

		queryLeagueBySummonerID(ID, &LoLStats::loadPlayerRank);
		queryGameBySummonerID(ID, &LoLStats::loadMatchHistory);
	}
	reply->deleteLater();
}

void LoLStats::loadPlayerRank() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());
	if (reply->error() == QNetworkReply::NoError) {
		QString json_res(reply->readAll());
		QJsonObject res = QJsonDocument::fromJson(json_res.toUtf8()).object();
		QJsonArray data = res[QString::number(ID)].toArray();

		for (auto it = data.begin(); it != data.end(); it++) {
			QString tier;
			QString queue;
			QString division;
			int LP;
			int wins;
			int losses;
			
			QJsonObject entry = it->toObject();
			tier = entry["tier"].toString();
			queue = entry["queue"].toString();
			QJsonArray players = entry["entries"].toArray();

			for (auto it2 = players.begin(); it2 != players.end(); it2++) {
				QMap<QString, QVariant> player = it2->toVariant().toMap();
				if (player["playerOrTeamId"].toInt() == ID) {
					// Player found
					division = player["division"].toString();
					LP = player["leaguePoints"].toInt();
					wins = player["wins"].toInt();
					losses = player["losses"].toInt();

					break;
				}
			}

			if (queue == "RANKED_SOLO_5x5") {	// Solo queue
				soloRank = STRING_TO_RANK[tier + division];
				soloLP = LP;
				soloWins = wins;
				soloLosses = losses;
			}
			else if (queue == "RANKED_FLEX_SR") {	// Flex queue
				flexRank = STRING_TO_RANK[tier + division];
				flexLP = LP;
				flexWins = wins;
				flexLosses = losses;
			}
			// TODO: Support twisted treeline stats
		}
	}
	reply->deleteLater();
}

void LoLStats::alertPlayerDataQueryFinished() {
	queryState += 1;
	if (queryState == queryStateMax) {
		loadStatsPage();
		switchStatsPage();
		queryState = 0;
	}
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// Functions for changing the UI >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
void LoLStats::loadStatsPage() {
	// Set username
	ui.usernameLabel->setText("<html><head/><body><p><span style=\" font - size:36pt; color:#ff0000; \">" + username + "</span></p></body></html>");		// Replace this way to keep rich text from QtCreator

	// Set solo queue UI
	ui.soloRankIcon->setPixmap(QPixmap(":/rankedIcons/Ranked Icons/" + RANK_TO_ICON[soloRank]));
	ui.soloRankLabel->setText("<html><head/><body><p><span style=\" font - size:26pt; color:#0000ff; \">" + RANK_TO_DISPLAY[soloRank] + "</span></p></body></html>");
	if (soloRank != UNRANKED) {
		ui.soloLPLabel->setVisible(true);
		ui.soloLPLabel->setText(QString::number(soloLP) + " LP");
	}
	else {
		ui.soloLPLabel->setVisible(false);
	}
	if (soloWins + soloLosses > 0) {
		ui.soloWinLossLabel->setVisible(true);
		ui.soloWinRateLabel->setVisible(true);
		ui.soloWinLossLabel->setText(QString::number(soloWins) + "W " + QString::number(soloLosses) + "L");
		ui.soloWinRateLabel->setText(QString::number((double)soloWins / (soloWins + soloLosses) * 100, 'g', 3) + "%");
	}
	else {
		ui.soloWinLossLabel->setVisible(false);
		ui.soloWinRateLabel->setVisible(false);
	}

	// Set flex queue UI
	ui.flexRankIcon->setPixmap(QPixmap(":/rankedIcons/Ranked Icons/" + RANK_TO_ICON[flexRank]));
	ui.flexRankLabel->setText("<html><head/><body><p><span style=\" font - size:26pt; color:#0000ff; \">" + RANK_TO_DISPLAY[flexRank] + "</span></p></body></html>");
	if (flexRank != UNRANKED) {
		ui.flexLPLabel->setVisible(true);
		ui.flexLPLabel->setText(QString::number(flexLP) + " LP");
	}
	else {
		ui.flexLPLabel->setVisible(false);
	}
	if (flexWins + flexLosses > 0) {
		ui.flexWinLossLabel->setVisible(true);
		ui.flexWinRateLabel->setVisible(true);
		ui.flexWinLossLabel->setText(QString::number(flexWins) + "W " + QString::number(flexLosses) + "L");
		ui.flexWinRateLabel->setText(QString::number((double)flexWins / (flexWins + flexLosses) * 100, 'g', 3) + "%");
	}
	else {
		ui.flexWinLossLabel->setVisible(false);
		ui.flexWinRateLabel->setVisible(false);
	}

	// Set match history
	for (auto it = games.begin(); it != games.end(); it++) {
		addGameToHistory(&*it);
	}
}

void LoLStats::loadMatchHistory() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());
	if (reply->error() == QNetworkReply::NoError) {
		QString json_res(reply->readAll());
		QJsonObject res = QJsonDocument::fromJson(json_res.toUtf8()).object();
		QJsonArray gamesData = res["games"].toArray();

		for (auto it = gamesData.begin(); it != gamesData.end(); it++) {
			QJsonObject gameInfo = it->toObject();
			QJsonObject stats = gameInfo["stats"].toObject();
			QJsonArray fellowPlayers = gameInfo["fellowPlayers"].toArray();

			Game game;
			game.gameType = gameInfo["subType"].toString();
			game.kills = stats["championsKilled"].toInt();
			game.deaths = stats["numDeaths"].toInt();
			game.assists = stats["assists"].toInt();
			game.setGameDate(gameInfo["createDate"].toDouble());
			game.win = stats["win"].toBool();
			game.gameLength = stats["timePlayed"].toInt();
			game.creepScore = stats["minionsKilled"].toInt() + stats["neutralMinionsKilled"].toInt();
			game.totalDamageDealtToChampions = stats["totalDamageDealtToChampions"].toInt();
			game.championID = gameInfo["championId"].toInt();
			championIDs.insert(game.championID);
			game.spell1ID = gameInfo["spell1"].toInt();
			summonerSpellIDs.insert(game.spell1ID);
			game.spell2ID = gameInfo["spell2"].toInt();
			summonerSpellIDs.insert(game.spell2ID);
			game.item1ID = stats["item0"].toInt();
			itemIDs.insert(game.item1ID);
			game.item2ID = stats["item1"].toInt();
			itemIDs.insert(game.item2ID);
			game.item3ID = stats["item2"].toInt();
			itemIDs.insert(game.item3ID);
			game.item4ID = stats["item3"].toInt();
			itemIDs.insert(game.item4ID);
			game.item5ID = stats["item4"].toInt();
			itemIDs.insert(game.item5ID);
			game.item6ID = stats["item5"].toInt();
			itemIDs.insert(game.item6ID);
			int team1Count = 0;
			int team2Count = 0;
			int teamID = gameInfo["teamId"].toInt();
			if (teamID == 100) {
				game.team1Players[team1Count].championID = game.championID;
				game.team1Players[team1Count].playerID = ID;
				team1Count++;
			}
			else {
				game.team2Players[team2Count].championID = game.championID;
				game.team2Players[team2Count].playerID = ID;
				team2Count++;
			}
			for (int i = 0; i < fellowPlayers.size(); i++) {
				QJsonObject playerInfo = fellowPlayers[i].toObject();
				int playerID = playerInfo["summonerId"].toInt();
				playerIDs.insert(playerID);
				int teamID = playerInfo["teamId"].toInt();
				int championID = playerInfo["championId"].toInt();
				championIDs.insert(championID);
				switch (teamID) {
					case 100:
						game.team1Players[team1Count].championID = championID;
						game.team1Players[team1Count].playerID = playerID;
						team1Count++;
						break;
					case 200:
						game.team2Players[team2Count].championID = championID;
						game.team2Players[team2Count].playerID = playerID;
						team2Count++;
						break;
				}
			}
			games.push_back(game);
		}

		prepareChampionIcons();
		prepareSummonerSpellIcons();
		prepareItemIcons();
		preparePlayerNames();
	}
	reply->deleteLater();
}

void LoLStats::loadGraph(int mode) {
	clearGraph();
	QLineSeries *series = new QLineSeries();
	
	if (mode == 0) {	// Graph average cs/min
		for (int i = 0; i < games.size(); i++) {
			double csPerMin = games[i].creepScore / ((double)games[i].gameLength / 60);
			series->append(games.size() - i - 1, csPerMin);
		}
	}

	QChart *chart = new QChart();
	chart->addSeries(series);
	chart->legend()->hide();
	chart->setTitle("CS/Min over past 10 games");

	QValueAxis *axisX = new QValueAxis;
	axisX->setTitleText("Games (Most recent on right)");
	axisX->setTickCount(10);
	axisX->setLabelsVisible(false);
	chart->addAxis(axisX, Qt::AlignBottom);
	series->attachAxis(axisX);

	QValueAxis *axisY = new QValueAxis;
	axisY->setMin(0);
	axisY->setLabelFormat("%i");
	axisY->setTitleText("CS/Min");
	chart->addAxis(axisY, Qt::AlignLeft);
	series->attachAxis(axisY);
	
	QChartView *chartView = new QChartView(chart);
	chartView->setRenderHint(QPainter::Antialiasing);
	
	ui.graphLayout->addWidget(chartView);
}

void LoLStats::switchSearchPage() {
	ui.indexStackedWidget->setCurrentIndex(0);
}

void LoLStats::switchStatsPage() {
	ui.indexStackedWidget->setCurrentIndex(1);
}

void LoLStats::addGameToHistory(Game* game) {
	LoLGame* matchWidgit = new LoLGame(game, this);
	ui.matchListContainer->layout()->addWidget(matchWidgit);
}

void LoLStats::clearHistory() {
	QLayoutItem* item;
	while (item = ui.matchListContainer->layout()->takeAt(0)) {
		delete item->widget();
		delete item;
	}
}

void LoLStats::clearGraph() {
	QLayoutItem* item;
	while (item = ui.graphLayout->takeAt(0)) {
		delete item->widget();
		delete item;
	}
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// Utility functions >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
void LoLStats::saveDataToFile(QByteArray data, QString filename) {
	QFile file(filename);
	file.open(QIODevice::WriteOnly);
	file.write(data);
	file.close();
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void LoLStats::debug() {
	loadGraph(0);
}