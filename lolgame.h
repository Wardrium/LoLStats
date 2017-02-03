#ifndef LOLGAME_H
#define LOLGAME_H

#include <QWidget>
#include "ui_lolgame.h"
#include "lolstats.h"
#include "loldata.h"

class LoLGame : public QWidget
{
	Q_OBJECT

public:
	LoLGame(QWidget *parent = 0);
	LoLGame(LoLData::Game* game, QWidget *parent = 0);
	~LoLGame();

private:
	Ui::LoLGame ui;

	void setChampionIcon(QLabel* label, int championID);
	void setSummonerSpellIcon(QLabel* label, int summonerSpellID);
	void setItemIcon(QLabel* label, int itemID);
	void setPlayerName(QLabel* label, int playerID);
};

#endif // LOLGAME_H
