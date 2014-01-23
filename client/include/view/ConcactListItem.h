#ifndef CONTACTLISTITEM_H
#define CONTACTLISTITEM_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QVariant>
#include <QSize>
#include <QPushButton>
#include <QStylePainter>
#include <QApplication>
#include <QMouseEvent>
#include <QDialog>
#include <iostream>

namespace Babibel
{
namespace View
{
class BabibelWindow;
}
}

#include "include/Enum.h"
#include "include/controller/controller.h"

namespace Babibel
{
namespace View
{
class ContactListItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

    QObject                 *_parent;
    QImage                  _callIcon;
    QImage                  _chatIcon;
    QImage                  _hangupIcon;
    QImage                  _cantCallIcon;

signals:
 int openConversation(const QString &login);
 int openConversationCall(const QString &login);

public:

    enum eElement
    {
        NICKNAME = Qt::UserRole + 1,
        TEXTSTATUS,
        STATUS
    };


    ContactListItemDelegate(QObject* parent = 0)
        : _parent(parent), _callIcon(":/Images/images/call.png"), _chatIcon(":/Images/images/chat.png"), _hangupIcon(":/Images/images/hangup.png"), _cantCallIcon(":/Images/images/cantcall.png")
    {
        connect(this, SIGNAL(openConversation(QString)), parent, SLOT(openConversation(QString)));
        connect(this, SIGNAL(openConversationCall(QString)), parent, SLOT(openConversationCall(QString)));
    }

    virtual ~ContactListItemDelegate()
    {
    }

    void	paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
    {
        if (option.state & QStyle::State_Selected)
            painter->fillRect(option.rect, QColor(255, 0, 0, 100));
        if (index.data(NICKNAME).canConvert<QString>())
        {
            QString v = index.data(NICKNAME).value<QString>();
            QFont font = painter->font();
            QFont bckp = font;
            font.setPointSize (15);
            font.setWeight(QFont::Bold);
            painter->setFont(font);
            painter->drawText(QRect(option.rect.topLeft() + QPoint(30, 0), option.rect.bottomRight()), v);
            painter->setFont(bckp);
        }
        if (index.data(TEXTSTATUS).canConvert<QString>())
        {
            QString v = index.data(TEXTSTATUS).value<QString>();
            painter->drawText(QRect(option.rect.topLeft() + QPoint(0, option.rect.height() / 2) + QPoint(30, 0), option.rect.bottomRight()), v);
        }
        if (index.data(STATUS).canConvert<eStatus>())
        {
            eStatus v = index.data(STATUS).value<enum eStatus>();
            painter->setRenderHint(QPainter::Antialiasing);
            switch (v) {
            case AVAILABLE:
                painter->setBrush(QBrush(QColor(0,255,0,150), Qt::SolidPattern));
                break;
            case BUSY:
                painter->setBrush(QBrush(QColor(255,0,0,150), Qt::SolidPattern));
                break;
            case AWAY:
                painter->setBrush(QBrush(QColor(255,255,0,150), Qt::SolidPattern));
                break;
            case DISCONNECTED:
                painter->setBrush(QBrush(QColor(0,0,0,42), Qt::SolidPattern));
                break;
            default:
                painter->setBrush(QBrush(QColor(255,255,255,255), Qt::SolidPattern));
                break;
            }
            painter->drawEllipse(option.rect.topLeft() + QPoint(0, option.rect.height() / 2) + QPoint(15, 0), 7, 7);
            //painter->drawText(QRect(option.rect.topLeft() + QPoint(0, option.rect.height() / 2), option.rect.bottomRight()), v);
        }

        //DRAW BTNTEXT
        QRect r = option.rect;
        painter->setRenderHint(QPainter::Antialiasing);

        QRect chatRect(r.left() + r.width() - r.height() / 2  - r.height() / 4 ,
                       r.top()  +  r.height() / 2 - r.height() / 4,
                       r.height() / 2,
                       r.height() / 2);
        painter->drawImage(chatRect, this->_chatIcon);

        //DRAW BTNCALL

        QRect callRect(r.left() + r.width() - r.height() / 2  - r.height() / 4  - r.height() / 2  - r.height() / 4,
                       r.top()  +  r.height() / 2 - r.height() / 4,
                       r.height() / 2,
                       r.height() / 2);


        switch (((::Babibel::View::BabibelWindow *)this->_parent)->_controller->model()->statusCall())
        {
        case NONE:
            painter->drawImage(callRect, this->_callIcon);
            break;
        case ISWAITING:
            painter->drawImage(callRect, this->_callIcon);
            break;
        case ISCALLING:
            if (((::Babibel::View::BabibelWindow *)this->_parent)->_controller->model()->callingLogin() == index.data(NICKNAME).value<QString>().toStdString())
               painter->drawImage(callRect, this->_hangupIcon);
            else
               painter->drawImage(callRect, this->_cantCallIcon);
            break;
        case ISRINGING:
            if (((::Babibel::View::BabibelWindow *)this->_parent)->_controller->model()->callingLogin() == index.data(NICKNAME).value<QString>().toStdString())
               painter->drawImage(callRect, this->_callIcon);
            else
               painter->drawImage(callRect, this->_cantCallIcon);
            break;
        default:
            painter->drawImage(callRect, this->_cantCallIcon);
            break;
        }

    }

    bool editorEvent(QEvent *event, QAbstractItemModel *, const QStyleOptionViewItem &option, const QModelIndex &index)
    {
        if( event->type() == QEvent::MouseButtonRelease )
        {
            QMouseEvent * e = (QMouseEvent *)event;

            QRect r = option.rect;

            QRect chatRect(r.left() + r.width() - r.height() / 2  - r.height() / 4 ,
                           r.top()  +  r.height() / 2 - r.height() / 4,
                           r.height() / 2,
                           r.height() / 2);
            QRect callRect(r.left() + r.width() - r.height() / 2  - r.height() / 4  - r.height() / 2  - r.height() / 4,
                           r.top()  +  r.height() / 2 - r.height() / 4,
                           r.height() / 2,
                           r.height() / 2);

            if (chatRect.contains(e->x(), e->y()) && index.data(NICKNAME).canConvert<QString>())
                emit openConversation(index.data(NICKNAME).value<QString>());
            else if (callRect.contains(e->x(), e->y()))
                emit openConversationCall(index.data(NICKNAME).value<QString>());
        }
        return (false);
    }

    QSize sizeHint(const QStyleOptionViewItem &option,
                                            const QModelIndex &) const
     {
        return (QSize(option.decorationSize.width(), option.decorationSize.height() * 2));
     }

};
}
}

Q_DECLARE_METATYPE(eStatus);

#endif // CONTACTLISTITEM_H
