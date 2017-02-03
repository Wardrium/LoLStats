#ifndef LOLSTATS_H
#define LOLSTATS_H

#include <QtWidgets/QMainWindow>
#include <QtNetwork>
#include "ui_lolstats.h"
#include "LoLData.h"

class LoLStats : public QWidget
{
	Q_OBJECT

public:
	LoLStats(QWidget *parent = 0);

private:
	Ui::LoLStatsClass ui;
	LoLData data;

	// Switch between pages
	void switchMainPage();
	
	// Get display ready
	void loadMainPage();
	void loadMatchHistory();
	void loadGraph(int mode);

	// Helper functions for getting display ready
	void addGameToHistory(LoLData::Game* game);
	void clearMatchHistory();
	void clearGraph();

private slots:
	void searchPlayer(); 	// Search player and load in required data
	void switchSearchPage();
	void debug();
};

#endif // LOLSTATS_H
