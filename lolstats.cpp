#include <QUrl>
#include <QDebug>
#include <QFileInfo>
#include <QChart>
#include <QLineSeries>
#include <QChartView>
#include <QValueAxis>

#include "lolstats.h"
#include "lolgame.h"

#define PLAYER data.mainPlayer

using QtCharts::QChart;
using QtCharts::QLineSeries;
using QtCharts::QChartView;
using QtCharts::QValueAxis;

LoLStats::LoLStats(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.searchButton, SIGNAL(clicked()), this, SLOT(searchPlayer()));
	connect(ui.newSearchButton, SIGNAL(clicked()), this, SLOT(switchSearchPage()));
	connect(ui.debugButton, SIGNAL(clicked()), this, SLOT(debug()));
}

// Search player and load in required data
void LoLStats::searchPlayer() {
	// Reset values
	QString username = ui.usernameText->text();
	data.prepareMainPage(username);
	loadMainPage();
	loadMatchHistory();
	switchMainPage();
}

// Switch between pages >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

void LoLStats::switchMainPage() {
	ui.indexStackedWidget->setCurrentIndex(1);
}

void LoLStats::switchSearchPage() {
	ui.indexStackedWidget->setCurrentIndex(0);
}


// Get display ready >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

void LoLStats::loadMainPage() {
	// Set username
	ui.usernameLabel->setText("<html><head/><body><p><span style=\" font - size:36pt; color:#ff0000; \">" + PLAYER.username + "</span></p></body></html>");

	// Set solo queue UI
	ui.soloRankIcon->setPixmap(QPixmap(":/rankedIcons/Ranked Icons/" + LoLData::RANK_TO_ICON[PLAYER.soloRank]));
	ui.soloRankLabel->setText("<html><head/><body><p><span style=\" font - size:26pt; color:#0000ff; \">" + LoLData::RANK_TO_DISPLAY[PLAYER.soloRank] + "</span></p></body></html>");
	if (PLAYER.soloRank != LoLData::UNRANKED) {
		ui.soloLPLabel->setVisible(true);
		ui.soloLPLabel->setText(QString::number(PLAYER.soloLP) + " LP");
	}
	else {
		ui.soloLPLabel->setVisible(false);
	}
	if (PLAYER.soloWins + PLAYER.soloLosses > 0) {
		ui.soloWinLossLabel->setVisible(true);
		ui.soloWinRateLabel->setVisible(true);
		ui.soloWinLossLabel->setText(QString::number(PLAYER.soloWins) + "W " + QString::number(PLAYER.soloLosses) + "L");
		ui.soloWinRateLabel->setText(QString::number((double)PLAYER.soloWins / (PLAYER.soloWins + PLAYER.soloLosses) * 100, 'g', 3) + "%");
	}
	else {
		ui.soloWinLossLabel->setVisible(false);
		ui.soloWinRateLabel->setVisible(false);
	}

	// Set flex queue UI
	ui.flexRankIcon->setPixmap(QPixmap(":/rankedIcons/Ranked Icons/" + LoLData::RANK_TO_ICON[PLAYER.flexRank]));
	ui.flexRankLabel->setText("<html><head/><body><p><span style=\" font - size:26pt; color:#0000ff; \">" + LoLData::RANK_TO_DISPLAY[PLAYER.flexRank] + "</span></p></body></html>");
	if (PLAYER.flexRank != LoLData::UNRANKED) {
		ui.flexLPLabel->setVisible(true);
		ui.flexLPLabel->setText(QString::number(PLAYER.flexLP) + " LP");
	}
	else {
		ui.flexLPLabel->setVisible(false);
	}
	if (PLAYER.flexWins + PLAYER.flexLosses > 0) {
		ui.flexWinLossLabel->setVisible(true);
		ui.flexWinRateLabel->setVisible(true);
		ui.flexWinLossLabel->setText(QString::number(PLAYER.flexWins) + "W " + QString::number(PLAYER.flexLosses) + "L");
		ui.flexWinRateLabel->setText(QString::number((double)PLAYER.flexWins / (PLAYER.flexWins + PLAYER.flexLosses) * 100, 'g', 3) + "%");
	}
	else {
		ui.flexWinLossLabel->setVisible(false);
		ui.flexWinRateLabel->setVisible(false);
	}
}

void LoLStats::loadMatchHistory() {
	// Set match history
	clearMatchHistory();
	for (auto it = data.games.begin(); it != data.games.end(); it++) {
		addGameToHistory(&*it);
	}
}

void LoLStats::loadGraph(int mode) {
	clearGraph();
	QLineSeries *series = new QLineSeries();
	
	if (mode == 0) {	// Graph average cs/min
		for (int i = 0; i < data.games.size(); i++) {
			double csPerMin = data.games[i].creepScore / ((double)data.games[i].gameLength / 60);
			series->append(data.games.size() - i - 1, csPerMin);
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


// Helper functions for getting display reaedy >>>>>>>>>>>>>>>>>>>>>>>>>>>

void LoLStats::addGameToHistory(LoLData::Game* game) {
	LoLGame* matchWidgit = new LoLGame(game, this);
	ui.matchListContainer->layout()->addWidget(matchWidgit);
}

void LoLStats::clearMatchHistory() {
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

void LoLStats::debug() {
	loadGraph(0);
}