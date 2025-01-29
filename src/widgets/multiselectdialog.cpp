#include "multiselectdialog.h"
#include <qtreeview.h>

MultiSelectDialog::MultiSelectDialog(QWidget *parent, const QString &caption, const QString &directory, const QString &filter) :
    QFileDialog(parent, caption, directory, filter)
{
    setFileMode(QFileDialog::Directory);
    setOption(QFileDialog::DontUseNativeDialog);

    internal_treeview = findChild<QTreeView*>("treeView");
    if (internal_treeview) {
        internal_treeview->setSelectionMode(QAbstractItemView::MultiSelection);
    }
}

std::optional<QStringList> MultiSelectDialog::getExistingDirectories()
{
    setOption(Option::ShowDirsOnly);
    setViewMode(ViewMode::Detail);
    if (exec() == QDialog::Accepted) {
        return std::make_optional<QStringList>(selectedFiles());
    }
    return std::nullopt;
}
