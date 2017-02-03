#pragma once
#include <QtWidgets/QMainWindow>
#include <QtNetwork>
#include <QUrl>
#include <QDebug>
#include <QFileInfo>
#include <QChart>
#include <QLineSeries>
#include <QChartView>
#include <QValueAxis>

class LoLData : public QWidget
{
	Q_OBJECT
public:
	enum Rank {
		UNRANKED, BRONZE_5, BRONZE_4, BRONZE_3, BRONZE_2, BRONZE_1,
		SILVER_5, SILVER_4, SILVER_3, SILVER_2, SILVER_1,
		GOLD_5, GOLD_4, GOLD_3, GOLD_2, GOLD_1,
		PLATINUM_5, PLATINUM_4, PLATINUM_3, PLATINUM_2, PLATINUM_1,
		DIAMOND_5, DIAMOND_4, DIAMOND_3, DIAMOND_2, DIAMOND_1, MASTER, CHALLENGER
	};
	struct MainPlayer {
		// Data about summoner
		QString username;
		int ID;
		int summonerLevel;
		Rank soloRank;
		int soloLP;
		int soloWins;
		int soloLosses;
		Rank flexRank;
		int flexLP;
		int flexWins;
		int flexLosses;

		MainPlayer() {
			reset();
		}

		void reset() {
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
		}
	};
	struct GamePlayer {
		int playerID;
		QString playerName;
		int championID;

		GamePlayer() : playerID(0), playerName("NOT FOUND"), championID(0) {}
	};
	struct Game {
		QString gameType;
		int kills, deaths, assists;
		double KDA;
		QDateTime gameDate;
		int championID;
		int spell1ID, spell2ID;
		int item1ID, item2ID, item3ID, item4ID, item5ID, item6ID;
		GamePlayer team1Players[5], team2Players[5];

		bool win;
		int creepScore;
		int gold;
		int totalDamageDealtToChampions;
		int gameLength;	// In seconds

		Game() {}
		void setGameDate(double msecs) {
			gameDate.setMSecsSinceEpoch(msecs);
		}
	};
	// Have different struct for each in case I want to add more data in later.
	struct Champion {
		QString name;
		QString imageName;

		Champion(QString name0, QString imageName0) : name(name0), imageName(imageName0) {}
		Champion() : imageName("NOT_FOUND.png") {}
	};
	struct SummonerSpell {
		QString name;
		QString imageName;

		SummonerSpell(QString name0, QString imageName0) : name(name0), imageName(imageName0) {}
		SummonerSpell() : imageName("NOT_FOUND.png") {}
	};
	struct Item {
		QString name;
		QString imageName;

		Item(QString name0, QString imageName0) : name(name0), imageName(imageName0) {}
		Item() : imageName("NOT_FOUND.png") {}
	};

	static const QHash<QString, Rank> STRING_TO_RANK;
	static const QHash<Rank, QString> RANK_TO_DISPLAY;
	static const QHash<Rank, QString> RANK_TO_ICON;
	static const QHash<QString, QString> GAME_TYPE_TO_DISPLAY;

	// Needs to be loaded in once from api
	static bool staticDataLoaded;
	static QHash<int, Champion> ID_TO_CHAMPION;
	static QHash<int, SummonerSpell> ID_TO_SUMMONER;
	static QHash<int, Item> ID_TO_ITEM;
	static QHash<int, QString> ID_TO_PLAYER;

	MainPlayer mainPlayer;	// Data on searched player
	QVector<Game> games;	// Match history data

	LoLData();

	// Load in the data neccessary for the view
	void prepareStaticData();	// Load in champion, summoner spell, and item ID to name mappings
	void prepareMainPage(const QString& username);
	void prepareIcons();
	void preparePlayerNames();

	// Data management
	void resetData();

private:
	QNetworkAccessManager qnam;
	QEventLoop loop;

	// IDs found in query so they can be downloaded if missing
	QSet<int> championIDs;
	QSet<int> summonerSpellIDs;
	QSet<int> itemIDs;
	QSet<int> playerIDs;

	// Helper functions for loading in data.
	void prepareChampionIcons();
	void prepareSummonerSpellIcons();
	void prepareItemIcons();

	// Riot API querying
	void querySummonerByName(const QString& name, void (LoLData::*callback)());
	void querySummonerByIDs(const QString& IDs, void (LoLData::*callback)());
	void queryLeagueBySummonerID(int ID, void (LoLData::*callback)());
	void queryGameBySummonerID(int ID, void(LoLData::*callback)());

	void queryChampionData(void (LoLData::*callback)());
	void querySummonerSpellData(void (LoLData::*callback)());
	void queryItemData(void (LoLData::*callback)());

	void queryChampionIcon(int ID, void (LoLData::*callback)());
	void querySummonerSpellIcon(int ID, void (LoLData::*callback)());
	void queryItemIcon(int ID, void (LoLData::*callback)());

	// Callback functions for loading in data after query
	void loadPlayerData();	// Load player ID and summoner level
	void loadPlayerNames();
	void loadPlayerRank();	// Load player rank
	void loadMatchHistory();

	void loadChampionData();	// Load champion ID to Champion map
	void loadSummonerSpellData();	// Load summoner spell ID to SummonerSpell map
	void loadItemData();		// Load item ID to Item map

	void downloadChampionIcon();	// Download champion icon from query
	void downloadSummonerSpellIcon();
	void downloadItemIcon();

	// Utility helpers
	void querySendRequest(QUrl url, void(LoLData::*callback)());
	void saveDataToFile(QByteArray data, QString filename);

signals:
	void httpRequestDone();
};