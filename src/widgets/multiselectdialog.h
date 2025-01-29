#pragma once

#include <QFileDialog>
#include <QTreeView>
#include <QListView>

class MultiSelectDialog : public QFileDialog
{
public:
    MultiSelectDialog(QWidget *parent = nullptr, const QString &caption = QString(), const QString &directory = QString(), const QString &filter = QString());

    std::optional<QStringList> getExistingDirectories();

private:
    // Qt internal objects
    QTreeView* internal_treeview = nullptr;
};
