#ifndef LOLSTATS_H
#define LOLSTATS_H

#include <QtWidgets/QMainWindow>
#include <QtNetwork>
#include <QLineSeries>

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
	QVector<LoLData::Game*> selectedGames;	// Selected games to be graphed

	// Switch between pages
	void switchMainPage();
	
	// Get display ready
	void loadMainPage();
	void loadMatchHistory();

	// Helper functions for getting display ready
	void loadCSperMinGraph(const QVector<LoLData::Game*>& games);
	void loadGoldperMinGraph(const QVector<LoLData::Game*>& games);
	void loadKDAGraph(const QVector<LoLData::Game*>& games);
	void setupGraph(QString title, QString axisXTitle, QString axisYTitle, QtCharts::QLineSeries* series);
	void addGameToHistory(LoLData::Game* game);
	void clearMatchHistory();
	void clearGraph();

private slots:	
	void loadGraph();
	void searchPlayer(); 	// Search player and load in required data
	void switchSearchPage();
};

#endif // LOLSTATS_H
