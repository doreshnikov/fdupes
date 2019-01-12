#include "duplicates_reciever.h"

duplicates_reciever::duplicates_reciever(QTreeWidget *widget, QPlainTextEdit *text, QProgressBar *bar) : _widget(widget), _text(text), _bar(bar),
                                                                                                         _duplicates(), _duplicates_count() {}

void duplicates_reciever::recieveDuplicateFile(const QString &origin, const QString &duplicate) {
    if (_duplicates.count(origin) == 0) {
        std::shared_ptr<QTreeWidgetItem> new_item(new QTreeWidgetItem());
        new_item->setExpanded(true);
        new_item->setText(0, "1");
        new_item->setText(1, QString::number(QFile(origin).size()) + "B");

        _duplicates[origin] = new_item;
        _duplicates_count[origin] = 1;

        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, origin);
        new_item->addChild(item);

        _widget->addTopLevelItem(new_item.get());
    }

    auto &origin_item = _duplicates[origin];

    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, duplicate);
    origin_item->addChild(item);
    origin_item->setText(0, QString::number(++_duplicates_count[origin]));
}

void duplicates_reciever::recieveProgress(const QString &file_name) {
    _text->appendPlainText(file_name);
    _bar->setValue(_bar->value() + 1);
}

void duplicates_reciever::clear() {
    _duplicates.clear();
    _duplicates_count.clear();

    _widget->clear();
    _text->clear();
    _bar->reset();
}
