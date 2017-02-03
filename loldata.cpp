#include "loldata.h"

// Block until http request is done without freezing UI.
#define WAIT_HTTP_REQUEST loop.exec();

const QString API_KEY = "RGAPI-b684def8-bd9c-4e15-bdc0-44017a9ab922";

// Map from Riot API string to Rank
const QHash<QString, LoLData::Rank> LoLData::STRING_TO_RANK({ { "BRONZEV", LoLData::BRONZE_5 },{ "BRONZEIV", LoLData::BRONZE_4 },
{ "BRONZEIII", LoLData::BRONZE_3 },{ "BRONZEII", LoLData::BRONZE_2 },{ "BRONZEI", LoLData::BRONZE_1 },
{ "SILVERV", LoLData::SILVER_5 },{ "SILVERIV", LoLData::SILVER_4 },{ "SILVERIII", LoLData::SILVER_3 },
{ "SILVERII", LoLData::SILVER_2 },{ "SILVERI", LoLData::SILVER_1 },{ "GOLDV", LoLData::GOLD_5 },
{ "GOLDIV", LoLData::GOLD_4 },{ "GOLDIII", LoLData::GOLD_3 },{ "GOLDII", LoLData::GOLD_2 },
{ "GOLDI", LoLData::GOLD_1 },{ "PLATINUMV", LoLData::PLATINUM_5 },{ "PLATINUMIV", LoLData::PLATINUM_4 },
{ "PLATINUMIII", LoLData::PLATINUM_3 },{ "PLATINUMII", LoLData::PLATINUM_2 },{ "PLATINUMI", LoLData::PLATINUM_1 },
{ "DIAMONDV", LoLData::DIAMOND_5 },{ "DIAMONDIV", LoLData::DIAMOND_4 },{ "DIAMONDIII", LoLData::DIAMOND_3 },
{ "DIAMONDII", LoLData::DIAMOND_2 },{ "DIAMONDI", LoLData::DIAMOND_1 },{ "MASTERI", LoLData::MASTER },
{ "CHALLENGERI", LoLData::CHALLENGER } });
// Map from Rank to display string
const QHash<LoLData::Rank, QString> LoLData::RANK_TO_DISPLAY({ { LoLData::BRONZE_5, "Bronze V" },{ LoLData::BRONZE_4, "Bronze IV" },
{ LoLData::BRONZE_3, "Bronze III" },{ LoLData::BRONZE_2, "Bronze II" },{ LoLData::BRONZE_1, "Bronze I" },
{ LoLData::SILVER_5, "Silver V" },{ LoLData::SILVER_4, "Silver IV" },{ LoLData::SILVER_3, "Silver III" },
{ LoLData::SILVER_2, "Silver II" },{ LoLData::SILVER_1, "Silver I" },{ LoLData::GOLD_5, "Gold V" },
{ LoLData::GOLD_4, "Gold IV" },{ LoLData::GOLD_3, "Gold III" },{ LoLData::GOLD_2, "Gold II" },
{ LoLData::GOLD_1, "Gold I" },{ LoLData::PLATINUM_5, "Platinum V" },{ LoLData::PLATINUM_4, "Platinum IV" },
{ LoLData::PLATINUM_3, "Platinum III" },{ LoLData::PLATINUM_2, "Platinum II" },{ LoLData::PLATINUM_1, "Platinum I" },
{ LoLData::DIAMOND_5, "Diamond V" },{ LoLData::DIAMOND_4, "Diamond IV" },{ LoLData::DIAMOND_3, "Diamond III" },
{ LoLData::DIAMOND_2, "Diamond II" },{ LoLData::DIAMOND_1, "Diamond I" },{ LoLData::MASTER, "Master" },
{ LoLData::CHALLENGER, "Challenger" },{ LoLData::UNRANKED, "Unranked" } });

// Map from Rank to icon png name
const QHash<LoLData::Rank, QString> LoLData::RANK_TO_ICON({ { LoLData::BRONZE_5, "bronze_5.png" },{ LoLData::BRONZE_4, "bronze_4.png" },
{ LoLData::BRONZE_3, "bronze_3.png" },{ LoLData::BRONZE_2, "bronze_2.png" },{ LoLData::BRONZE_1, "bronze_1.png" },
{ LoLData::SILVER_5, "silver_5.png" },{ LoLData::SILVER_4, "silver_4.png" },{ LoLData::SILVER_3, "silver_3.png" },
{ LoLData::SILVER_2, "silver_2.png" },{ LoLData::SILVER_1, "silver_1.png" },{ LoLData::GOLD_5, "gold_5.png" },
{ LoLData::GOLD_4, "gold_4.png" },{ LoLData::GOLD_3, "gold_3.png" },{ LoLData::GOLD_2, "gold_2.png" },
{ LoLData::GOLD_1, "gold_1.png" },{ LoLData::PLATINUM_5, "platinum_5.png" },{ LoLData::PLATINUM_4, "platinum_4.png" },
{ LoLData::PLATINUM_3, "platinum_3.png" },{ LoLData::PLATINUM_2, "platinum_2.png" },{ LoLData::PLATINUM_1, "platinum_1.png" },
{ LoLData::DIAMOND_5, "diamond_5.png" },{ LoLData::DIAMOND_4, "diamond_4.png" },{ LoLData::DIAMOND_3, "diamond_3.png" },
{ LoLData::DIAMOND_2, "diamond_2.png" },{ LoLData::DIAMOND_1, "diamond_1.png" },{ LoLData::MASTER, "master.png" },
{ LoLData::CHALLENGER, "challenger.png" },{ LoLData::UNRANKED, "unranked.png" } });

// Map from game subType string to display string.
const QHash<QString, QString> LoLData::GAME_TYPE_TO_DISPLAY({ { "RANKED_SOLO_5x5", "Ranked Solo" },
{ "RANKED_FLEX_SR", "Ranked Flex" },{ "NORMAL", "Normal" },{ "ARAM_UNRANKED", "ARAM" },
{ "RANKED_TEAM_3x3", "Ranked TT" },{ "NORMAL_3x3", "Twisted Treeline" } });

bool LoLData::staticDataLoaded = false;
QHash<int, LoLData::Champion> LoLData::ID_TO_CHAMPION;
QHash<int, LoLData::SummonerSpell> LoLData::ID_TO_SUMMONER;
QHash<int, LoLData::Item> LoLData::ID_TO_ITEM;
QHash<int, QString> LoLData::ID_TO_PLAYER;

LoLData::LoLData() {
	prepareStaticData();
	loop.connect(this, SIGNAL(httpRequestDone()), SLOT(quit()));
}

void LoLData::prepareStaticData() {
	if (staticDataLoaded == false) {
		queryChampionData(&LoLData::loadChampionData);
		querySummonerSpellData(&LoLData::loadSummonerSpellData);
		queryItemData(&LoLData::loadItemData);
		staticDataLoaded = true;
	}
}

void LoLData::prepareMainPage(const QString& username) {
	querySummonerByName(username, &LoLData::loadPlayerData);	// Get summoner ID
	WAIT_HTTP_REQUEST
	queryLeagueBySummonerID(mainPlayer.ID, &LoLData::loadPlayerRank);	// Prepare summoner rank info
	queryGameBySummonerID(mainPlayer.ID, &LoLData::loadMatchHistory);	// Prepare match history
	WAIT_HTTP_REQUEST
	WAIT_HTTP_REQUEST	// Finish above two functions before continuing
	prepareIcons();
	preparePlayerNames();
	WAIT_HTTP_REQUEST
}

void LoLData::prepareIcons() {
	prepareChampionIcons();
	prepareSummonerSpellIcons();
	prepareItemIcons();
}

void LoLData::preparePlayerNames() {
	QString queryIDs = "";
	for (auto it = playerIDs.begin(); it != playerIDs.end(); it++) {
		queryIDs += QString::number(*it) + ",";
	}
	queryIDs.remove(queryIDs.length() - 1);	// Remove last comma
	querySummonerByIDs(queryIDs, &LoLData::loadPlayerNames);
}

void LoLData::resetData() {
	mainPlayer.reset();

	championIDs.clear();
	summonerSpellIDs.clear();
	itemIDs.clear();
	playerIDs.clear();
	games.clear();
}

void LoLData::prepareChampionIcons() {
	for (auto it = championIDs.begin(); it != championIDs.end(); it++) {
		if (!QFileInfo::exists("Resources/Champion Icons/" + ID_TO_CHAMPION[*it].imageName)) {
			queryChampionIcon(*it, &LoLData::downloadChampionIcon);
			WAIT_HTTP_REQUEST
		}
	}
}

void LoLData::prepareSummonerSpellIcons() {
	for (auto it = summonerSpellIDs.begin(); it != summonerSpellIDs.end(); it++) {
		if (!QFileInfo::exists("Resources/Summoner Spell Icons/" + ID_TO_SUMMONER[*it].imageName)) {
			querySummonerSpellIcon(*it, &LoLData::downloadSummonerSpellIcon);
			WAIT_HTTP_REQUEST
		}
	}
}

void LoLData::prepareItemIcons() {
	for (auto it = itemIDs.begin(); it != itemIDs.end(); it++) {
		if (!QFileInfo::exists("Resources/Item Icons/" + ID_TO_ITEM[*it].imageName)) {
			queryItemIcon(*it, &LoLData::downloadItemIcon);
			WAIT_HTTP_REQUEST
		}
	}
}

// Query Riot API >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

// Fetch data
void LoLData::queryChampionData(void (LoLData::*callback)()) {
	QUrl url("http://ddragon.leagueoflegends.com/cdn/7.2.1/data/en_US/champion.json");
	querySendRequest(url, callback);
}

void LoLData::querySummonerSpellData(void (LoLData::*callback)()) {
	QUrl url("http://ddragon.leagueoflegends.com/cdn/7.2.1/data/en_US/summoner.json");
	querySendRequest(url, callback);
}

void LoLData::queryItemData(void (LoLData::*callback)()) {
	QUrl url("http://ddragon.leagueoflegends.com/cdn/7.2.1/data/en_US/item.json");
	querySendRequest(url, callback);
}

// Fetch icon png files
void LoLData::queryChampionIcon(int ID, void (LoLData::*callback)()) {
	QUrl url("http://ddragon.leagueoflegends.com/cdn/7.2.1/img/champion/" + ID_TO_CHAMPION[ID].imageName);
	querySendRequest(url, callback);
}

void LoLData::querySummonerSpellIcon(int ID, void (LoLData::*callback)()) {
	QUrl url("http://ddragon.leagueoflegends.com/cdn/7.2.1/img/spell/" + ID_TO_SUMMONER[ID].imageName);
	querySendRequest(url, callback);
}

void LoLData::queryItemIcon(int ID, void (LoLData::*callback)()) {
	QUrl url("http://ddragon.leagueoflegends.com/cdn/7.2.1/img/item/" + ID_TO_ITEM[ID].imageName);
	querySendRequest(url, callback);
}


// Load in data after query >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

// Find player and match data
void LoLData::querySummonerByName(const QString& username, void (LoLData::*callback)()) {
	QUrl url("https://na.api.pvp.net/api/lol/na/v1.4/summoner/by-name/" + username + "?api_key=" + API_KEY);
	querySendRequest(url, callback);
}

void LoLData::querySummonerByIDs(const QString& IDs, void (LoLData::*callback)()) {
	QUrl url("https://na.api.pvp.net/api/lol/na/v1.4/summoner/" + IDs + "?api_key=" + API_KEY);
	querySendRequest(url, callback);
}

void LoLData::queryLeagueBySummonerID(int ID, void (LoLData::*callback)()) {
	QUrl url("https://na.api.pvp.net/api/lol/na/v2.5/league/by-summoner/" + QString::number(ID) + "?api_key=" + API_KEY);
	querySendRequest(url, callback);
}

void LoLData::queryGameBySummonerID(int ID, void(LoLData::*callback)()) {
	QUrl url("https://na.api.pvp.net/api/lol/na/v1.3/game/by-summoner/" + QString::number(ID) + "/recent?api_key=" + API_KEY);
	querySendRequest(url, callback);
}

// Helper function for queries
void LoLData::querySendRequest(QUrl url, void(LoLData::*callback)()) {
	QNetworkReply* reply = qnam.get(QNetworkRequest(url));	// Makes the get request
	connect(reply, &QNetworkReply::finished, this, callback);
}


// Callback functions for loading in data after a query >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

void LoLData::loadPlayerData() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());
	if (reply->error() == QNetworkReply::NoError) {
		QString json_res(reply->readAll());
		QJsonObject res = QJsonDocument::fromJson(json_res.toUtf8()).object();
		QJsonObject data = res.begin()->toObject();

		mainPlayer.username = data["name"].toString();
		mainPlayer.ID = data["id"].toInt();
		mainPlayer.summonerLevel = data["summonerLevel"].toInt();
	}
	reply->deleteLater();
	emit httpRequestDone();
}

void LoLData::loadPlayerNames() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());
	if (reply->error() == QNetworkReply::NoError) {
		QString json_res(reply->readAll());
		QJsonObject data = QJsonDocument::fromJson(json_res.toUtf8()).object();
		ID_TO_PLAYER[mainPlayer.ID] = mainPlayer.username;
		for (auto it = data.begin(); it != data.end(); it++) {
			int ID = it.key().toInt();
			QString name = it->toObject()["name"].toString();
			ID_TO_PLAYER[ID] = name;
		}
	}
	reply->deleteLater();
	emit httpRequestDone();
}

void LoLData::loadPlayerRank() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());
	if (reply->error() == QNetworkReply::NoError) {
		QString json_res(reply->readAll());
		QJsonObject res = QJsonDocument::fromJson(json_res.toUtf8()).object();
		QJsonArray data = res[QString::number(mainPlayer.ID)].toArray();

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
				if (player["playerOrTeamId"].toInt() == mainPlayer.ID) {
					// Player found
					division = player["division"].toString();
					LP = player["leaguePoints"].toInt();
					wins = player["wins"].toInt();
					losses = player["losses"].toInt();

					break;
				}
			}

			if (queue == "RANKED_SOLO_5x5") {	// Solo queue
				mainPlayer.soloRank = STRING_TO_RANK[tier + division];
				mainPlayer.soloLP = LP;
				mainPlayer.soloWins = wins;
				mainPlayer.soloLosses = losses;
			}
			else if (queue == "RANKED_FLEX_SR") {	// Flex queue
				mainPlayer.flexRank = STRING_TO_RANK[tier + division];
				mainPlayer.flexLP = LP;
				mainPlayer.flexWins = wins;
				mainPlayer.flexLosses = losses;
			}
			// TODO: Support twisted treeline stats
		}
	}
	reply->deleteLater();
	emit httpRequestDone();
}

void LoLData::loadMatchHistory() {
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
			game.KDA = (double)(game.kills + game.assists) / game.deaths;
			game.setGameDate(gameInfo["createDate"].toDouble());
			game.win = stats["win"].toBool();
			game.gameLength = stats["timePlayed"].toInt();
			game.creepScore = stats["minionsKilled"].toInt() + stats["neutralMinionsKilled"].toInt();
			game.gold = stats["goldEarned"].toInt();
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
				game.team1Players[team1Count].playerID = mainPlayer.ID;
				team1Count++;
			}
			else {
				game.team2Players[team2Count].championID = game.championID;
				game.team2Players[team2Count].playerID = mainPlayer.ID;
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
	}
	reply->deleteLater();
	emit httpRequestDone();
}

void LoLData::loadChampionData() {
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
	emit httpRequestDone();
}

void LoLData::loadSummonerSpellData() {
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
	emit httpRequestDone();
}

void LoLData::loadItemData() {
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
	emit httpRequestDone();
}

void LoLData::downloadChampionIcon() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());
	if (reply->error() == QNetworkReply::NoError) {
		QByteArray imageData = reply->readAll();
		saveDataToFile(imageData, "Resources/Champion Icons/" + reply->url().fileName());
	}
	reply->deleteLater();
	emit httpRequestDone();
}

void LoLData::downloadSummonerSpellIcon() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());
	if (reply->error() == QNetworkReply::NoError) {
		QByteArray imageData = reply->readAll();
		saveDataToFile(imageData, "Resources/Summoner Spell Icons/" + reply->url().fileName());
	}
	reply->deleteLater();
	emit httpRequestDone();
}

void LoLData::downloadItemIcon() {
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());
	if (reply->error() == QNetworkReply::NoError) {
		QByteArray imageData = reply->readAll();
		saveDataToFile(imageData, "Resources/Item Icons/" + reply->url().fileName());
	}
	reply->deleteLater();
	emit httpRequestDone();
}

// Utility functions >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
void LoLData::saveDataToFile(QByteArray data, QString filename) {
	QFile file(filename);
	file.open(QIODevice::WriteOnly);
	file.write(data);
	file.close();
}