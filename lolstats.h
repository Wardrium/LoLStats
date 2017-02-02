#ifndef LOLSTATS_H
#define LOLSTATS_H

#include <QtWidgets/QMainWindow>
#include <QtNetwork>
#include "ui_lolstats.h"

extern const QString API_KEY;

class LoLStats : public QWidget
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
	struct GamePlayer {
		int playerID;
		QString playerName;
		int championID;

		GamePlayer() : playerID(0), playerName("NOT FOUND"), championID(0) {}
	};
	struct Game {
		QString gameType;
		int kills, deaths, assists;
		QDateTime gameDate;
		int championID;
		int spell1ID, spell2ID;
		int item1ID, item2ID, item3ID, item4ID, item5ID, item6ID;
		GamePlayer team1Players[5], team2Players[5];
		
		bool win;
		int creepScore;
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

	// Needs to be loaded in once from api
	static QHash<int, Champion> ID_TO_CHAMPION;
	static QHash<int, SummonerSpell> ID_TO_SUMMONER;
	static QHash<int, Item> ID_TO_ITEM;
	static QHash<int, QString> ID_TO_PLAYER;
	
	QSet<int> championIDs;	// Champion IDs that were found in match history.
	QSet<int> summonerSpellIDs;
	QSet<int> itemIDs;
	QSet<int> playerIDs;

	LoLStats(QWidget *parent = 0);
	~LoLStats();

private:
	Ui::LoLStatsClass ui;
	QNetworkAccessManager qnam;

	int numChampionIcons;
	int numSummonerSpellIcons;
	int numItemIcons;
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

	// Match history data
	QVector<Game> games;

	void prepareChampionIcons();	// Make sure all champion icons are downloaded. If not, will download them.
	void prepareSummonerSpellIcons();
	void prepareItemIcons();
	void preparePlayerNames();

	// Query Riot API
	void queryChampionData(void (LoLStats::*callback)());
	void querySummonerSpellData(void (LoLStats::*callback)());
	void queryItemData(void (LoLStats::*callback)());

	void queryChampionIcon(int ID, void (LoLStats::*callback)());
	void querySummonerSpellIcon(int ID, void (LoLStats::*callback)());
	void queryItemIcon(int ID, void (LoLStats::*callback)());

	void querySummonerByName(const QString& name, void (LoLStats::*callback)());
	void querySummonerByIDs(const QString& IDs, void (LoLStats::*callback)());
	void queryLeagueBySummonerID(int ID, void (LoLStats::*callback)());
	void queryGameBySummonerID(int ID, void(LoLStats::*callback)());

	void querySendRequest(QUrl url, void(LoLStats::*callback)());

	// Load data from querying API
	void loadChampionData();	// Load champion ID to Champion map
	void loadSummonerSpellData();	// Load summoner spell ID to SummonerSpell map
	void loadItemData();		// Load item ID to Item map
	void downloadChampionIcon();	// Download champion icon from query
	void downloadSummonerSpellIcon();
	void downloadItemIcon();
	void loadChampionIcons();	// Load champion icons. Assumes all champion icons are in resources folder.
	void loadSummonerSpellIcons();
	void loadItemIcons();
	void loadPlayerNames();
	void loadPlayerData();	// Load player ID and summoner level
	void loadPlayerRank();	// Load player rank

	void alertPlayerDataQueryFinished(); // Called whenever a query is finished. When all queries are finished, will load stats page
	int queryState = 0;	// Used to keep track of when all queries are finished and UI is ready to load.
	const int queryStateMax = 4;	// Load UI when queryState is equal to this.

	// Change UI
	void loadStatsPage();
	void loadMatchHistory();
	void loadGraph(int mode);
	void switchStatsPage();

	// Utility helpers
	void saveDataToFile(QByteArray data, QString filename);
	

private slots:
	void searchPlayer();
	void switchSearchPage();
	void addGameToHistory(Game* game);
	void clearHistory();
	void clearGraph();
	void debug();
};

#endif // LOLSTATS_H
