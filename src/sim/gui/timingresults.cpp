#include "timingresults.h"

#include <QVariant>
#include <QMessageBox>
#include <QtCore/QFile>
#include <QTextStream>

#include "../clocksdispatcher.h"

/*
 *  Constructs a TimingResults as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
TimingResults::TimingResults(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    setupUi(this);
    setObjectName(name);
    setModal(modal);

    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    QObject::connect(saveButton, SIGNAL(clicked()), this, SLOT(save()));
    QObject::connect(closeButton, SIGNAL(clicked()), this, SLOT(reject()));

}

/*
 *  Destroys the object and frees any allocated resources
 */
TimingResults::~TimingResults()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void TimingResults::languageChange()
{
    retranslateUi(this);
}

/*! \class TimingResults
 *   \brief Brief description.
 *         Brief description continued.
 *
 *  Detailed description starts here.
 */

void TimingResults::init(ClocksDispatcher* dispatch)
{
    dispatcher = dispatch;
}

void TimingResults::write( void )
{

    uint i = 0;
    QString s_counter, s_percent;
    int cnt_total_time = dispatcher->total_time.getCounter();
    int total_simulated = dispatcher->count_step*dispatcher->real_advance_period;
    typedef QHash<QString, NDClock*> UserClocks;
    QHashIterator<QString, UserClocks*> it(dispatcher->users);
    QTableWidgetItem* item;
    while(it.hasNext()) {
        QString user_name = it.peekNext().key();
        UserClocks userClocks = *(it.next().value());

        table->insertRow(i);
        item = new QTableWidgetItem(user_name);
        table->setItem(i, 0, item);
//        table->insertRows(i);
//        table->setText(i, 0, user_name);

        QHashIterator<QString, NDClock*> itc(userClocks);
        NDClock* sim_clock = userClocks[tr("Simulation")];
        item = new QTableWidgetItem(tr("Simulation step"));
        table->setItem(i, 1, item);
//        table->setText(i, 1, tr("Simulation step"));
        int cnt_sim_time = sim_clock->getCounter();
        s_counter.setNum(double(cnt_sim_time)/dispatcher->count_step, 'f', 4);
        item = new QTableWidgetItem(s_counter);
        table->setItem(i, 3, item);
//        table->setText(i, 3, s_counter);
        s_percent = QString::number(double(cnt_sim_time)/cnt_total_time*100.0, 'f', 3);
        item = new QTableWidgetItem(s_percent);
        table->setItem(i, 4, item);
//        table->setText(i, 4, s_percent);
        ++i;

        while(itc.hasNext()) {
            QString key = itc.peekNext().key();
            NDClock* clock = itc.next().value();
            if(sim_clock != clock) {
                table->insertRow(i);
                item = new QTableWidgetItem(key);
                table->setItem(i, 2, item);
//                table->insertRows(i);
//                table->setText(i, 2, key);
                int cnt = clock->getCounter();
                s_counter.setNum(double(cnt)/dispatcher->count_step, 'f', 4);
                item = new QTableWidgetItem(s_counter);
                table->setItem(i, 3, item);
//                table->setText(i, 3, s_counter);
                s_percent = QString::number(double(cnt)/cnt_sim_time*100.0, 'f', 3);
                item = new QTableWidgetItem(s_percent);
                table->setItem(i, 4, item);
//                table->setText(i, 4, s_percent);
            }
            else
                --i;
            ++i;
        }
        table->insertRow(i);
        item = new QTableWidgetItem("");
        table->setItem(i, 0, item);
//        table->insertRows(i);
//        table->setText(i, 0, "");
        ++i;
    }
    table->insertRow(i);
    item = new QTableWidgetItem(tr("General"));
    table->setItem(i, 0, item);
//    table->insertRows(i);
//    table->setText(i, 0, "General");
    item = new QTableWidgetItem(tr("Main loop step"));
    table->setItem(i, 1, item);
//    table->setText(i, 1, tr("Main loop step"));
    s_counter.setNum(double(cnt_total_time)/dispatcher->count_step, 'f', 4);
    item = new QTableWidgetItem(s_counter);
    table->setItem(i, 3, item);
//    table->setText(i, 3, s_counter);
    s_percent = QString::number(double(cnt_total_time)/
                            total_simulated*100.0, 'f', 3);
    item = new QTableWidgetItem(s_percent);
    table->setItem(i, 4, item);
//    table->setText(i, 4, s_percent);
    ++i;

    table->insertRow(i);
    item = new QTableWidgetItem(tr("Total simulated"));
    table->setItem(i, 1, item);
//    table->insertRows(i);
//    table->setText(i, 0, tr("Total simulated"));
    QTime total_simulated_time(0,0);
    total_simulated_time = total_simulated_time.addMSecs(total_simulated);
    item = new QTableWidgetItem(total_simulated_time.toString("h:mm:ss:zzz"));
    table->setItem(i, 3, item);
//    table->setText(i, 3, total_simulated_time.toString("h:mm:ss:zzz"));
    ++i;

    table->insertRow(i);
    item = new QTableWidgetItem(tr("Number of steps"));
    table->setItem(i, 1, item);
//    table->insertRows(i);
//    table->setText(i, 1, tr("Number of steps"));
    item = new QTableWidgetItem(QString::number(dispatcher->count_step));
    table->setItem(i, 3, item);
//    table->setText(i, 3, QString::number(dispatcher->count_step));

//    for(int i = 0; i < table->numCols(); i++)
//        table->adjustColumn(i);

}


void TimingResults::save( void )
{

    QFile file(dispatcher->file_name);
    int row_cnt = table->rowCount();
    int col_cnt = table->columnCount();
    if(file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        QTextStream ts(&file);
        if(file.size() > 0)
            ts << "\n\n\n";
//        ts << "# " << QDateTime::currentDateTime().
//              toString(Qt::LocalDate) << "\n\n";
        QLocale locale = QLocale();
        QDateTime date = QDateTime::currentDateTime();
        ts << "# " << locale.toString(date) << "\n\n";
        for(int i = 0; i < row_cnt; i++) {
            for(int j = 0; j < col_cnt; j++) {
//                QString t_text = table->text(i, j);
                QTableWidgetItem* item = table->item(i, j);
                QString t_text;
                if(!item)
                    t_text = "";
                else
                    t_text = item->text();
                if(j == 3 && !t_text.isEmpty() && i < row_cnt - 2)
                    t_text.append("ms.");
                if(j == 4 && !t_text.isEmpty() && i < row_cnt - 2)
                    t_text.append("%");
                if(j < col_cnt - 1) {
                    if(j == 3)
                        ts << t_text.rightJustified(10) << "\t";
                    else if(j == 1)
                        ts << t_text.leftJustified(20) << "\t";
                    else
                        ts << t_text.leftJustified(15) << "\t";
                }
                else
                    ts << t_text.rightJustified(10) << "\n";
            }
        }
        file.close();
    }
    else
//        QMessageBox::warning(nullptr,tr("Timing results"),
//                             tr("Couldn't open file"), tr("&Ok"));
        QMessageBox::warning(nullptr,tr("Timing results"),
                             tr("Couldn't open file"), QMessageBox::Ok);

}
