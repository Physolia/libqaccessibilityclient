/*
    Copyright 2012 Frederik Gladhorn <gladhorn@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QACCESSIBILITYCLIENT_REGISTRY_H
#define QACCESSIBILITYCLIENT_REGISTRY_H

#include <qobject.h>

#include "qaccessibilityclient_export.h"
#include "accessibleobject.h"
#include <QUrl>

#define accessibleRegistry (QAccessibleClient::Registry::instance())

namespace QAccessibleClient {

class RegistryPrivate;

/**
    This class represents the global accessibility registry.

    It provides information about running applications.
    All updates of accessible objects will result in signals emitted by this class.
*/
class QACCESSIBILITYCLIENT_EXPORT Registry : public QObject
{
    Q_OBJECT
    Q_ENUMS(EventListener)
    Q_ENUMS(CacheType)

public:

    enum EventListener {
        NoEventListeners = 0x0,             /*!< No event listeners registered or wanted */
        Window = 0x1,                       /*!< Window changes, such as new applications being started */
        Focus = 0x2,                        /*!< Focus listener reacts to focus changes - see signal \sa focusChanged */
        //FocusPoint = 0x4,

        //BoundsChanged = 0x8,
        //LinkSelected = 0x10,
        StateChanged = 0x20,                /*!< State of the accessible changed - see signal \sa stateChanged */
        ChildrenChanged = 0x40,             /*!< Children changed - see signal \sa childrenChanged */
        VisibleDataChanged = 0x80,          /*!< Visibility of the accessible changed - see signal \sa visibleDataChanged */
        SelectionChanged = 0x100,           /*!< Selection changed  - see signal \sa selectionChanged */
        ModelChanged = 0x200,               /*!< The model changed - see signal \sa modelChanged */

        TextCaretMoved = 0x400,             /*!< The text caret moved its position - see signal \sa textCaretMoved */
        TextChanged = 0x800,                /*!< The text changed - see signal \sa textChanged */
        TextSelectionChanged = 0x1000,      /*!< The text selection changed - see signal \sa textSelectionChanged */
        PropertyChanged = 0x2000,           /*!< A property changed. See signals \sa accessibleNameChanged and \sa accessibleDescriptionChanged */
        //TextBoundsChanged = 0x2000,
        //TextAttributesChanged = 0x4000,
        //AttributesChanged = 0x8000,

        AllEventListeners = 0xffffffff      /*!< All possible event listeners */
    };
    Q_DECLARE_FLAGS(EventListeners, EventListener)

    enum CacheType {
        NoCache, ///< Disable any caching.
        WeakCache, ///< Cache only objects in use and free them as long as no-one holds a reference to them any longer.
        StrongCache ///< Cache all objects forever and only free them once they got explicitly removed.
    };

    CacheType cacheType() const;
    void setCacheType(CacheType type);

    AccessibleObject clientCacheObject(const QString &id) const;
    QStringList clientCacheObjects() const;
    void clearClientCache();

    explicit Registry(QObject *parent = 0);
    virtual ~Registry();

public slots:

    /**
        Returns true if the accessibility stack is enabled.

        This means that an atspi registry daemon is running and available as
        org.a11y.bus at the session dbus. The org.a11y.bus implments the
        org.a11y.Status dbus interface that offers the IsEnabled property. The
        \a isEnabled and \a setEnabled methods do read/write the boolean value
        of that org.a11y.Status.IsEnabled dbus property..
    */
    bool isEnabled() const;
    void setEnabled(bool enable);

    /**
        In order to get notified of changes in accessible applications
        it is neccessary to subscribe to the listeners that are relevant.

        This will unsubscribe all previously subscribed event listeners.
    */
    void subscribeEventListeners(const EventListeners &listeners) const;
    EventListeners subscribedEventListeners() const;

    /**
        List of all currently running applications that
        expose an accessibility interface.
    */
    QList<AccessibleObject> applications() const;

    /**
        Returns a QUrl that references the AccessibleObject.

        This can be used to serialize/unserialize an AccessibleObject
        to pass it around as string and restore the AccessibleObject
        later on.

        The returned QUrl returns a scheme of "AccessibleObject", the
        dbus path as url path and the dbus service as url fragment.
    */
    QUrl url(const AccessibleObject &object) const;
    AccessibleObject fromUrl(const QUrl &url) const;

Q_SIGNALS:

    /**
        Emitted if the \a isEnabled state changed.

        If the accessibility daemon got enabled or disabled either by us or
        ny someone else then this signal is emitted.
    */
    void enabledChanged(bool enabled);

    /**
        Emitted if a AccessibleObject is created.

        This signal indicates that the \a AccessibleObject instance was
        created and is going to be added/attached.
    */
    void added(const QAccessibleClient::AccessibleObject &object);

    /**
        Emitted if a AccessibleObject is destroyed.

        This signal indicates that the \a AccessibleObject instance was
        destroyed and ended its life-time.
    */
    void removed(const QAccessibleClient::AccessibleObject &object);

    /**
        Emitted if a AccessibleObject is marked defunct.

        This signal indicates that the \a AccessibleObject became invalid
        and does not point any longer to any valid accessible object.
    */
    void defunct(const QAccessibleClient::AccessibleObject &object);

    void windowCreated(const QAccessibleClient::AccessibleObject &object);
    void windowDestroyed(const QAccessibleClient::AccessibleObject &object);
    void windowClosed(const QAccessibleClient::AccessibleObject &object);
    void windowReparented(const QAccessibleClient::AccessibleObject &object);
    void windowMinimized(const QAccessibleClient::AccessibleObject &object);
    void windowMaximized(const QAccessibleClient::AccessibleObject &object);
    void windowRestored(const QAccessibleClient::AccessibleObject &object);
    void windowActivated(const QAccessibleClient::AccessibleObject &object);
    void windowDeactivated(const QAccessibleClient::AccessibleObject &object);
    void windowDesktopCreated(const QAccessibleClient::AccessibleObject &object);
    void windowDesktopDestroyed(const QAccessibleClient::AccessibleObject &object);
    void windowRaised(const QAccessibleClient::AccessibleObject &object);
    void windowLowered(const QAccessibleClient::AccessibleObject &object);
    void windowMoved(const QAccessibleClient::AccessibleObject &object);
    void windowResized(const QAccessibleClient::AccessibleObject &object);
    void windowShaded(const QAccessibleClient::AccessibleObject &object);
    void windowUnshaded(const QAccessibleClient::AccessibleObject &object);

    //void boundsChanged(const QAccessibleClient::AccessibleObject &object);
    //void linkSelected(const QAccessibleClient::AccessibleObject &object);

    /**
        \brief Notifies about a state change in an object.

        The \a state of \a object has change.
        If the state is now set \a active is true, otherwise the state was removed.
     */
    void stateChanged(const QAccessibleClient::AccessibleObject &object, const QString &state, bool active);

    /**
        \brief Notifies about a new AccessibleObject

        The childAdded signal is emitted when a new accessible object is created.
        This signal depends on the implementation of the server side and is not
        reliable for all applications.
        The parameter \a childIndex is the index of the child that has been added.
        \sa AccessibleObject::child(), childRemoved()
     */
    void childAdded(const QAccessibleClient::AccessibleObject &parent, int childIndex);

    /**
        \brief Notifies that an AccessibleObject has been removed

        The parameter \a childIndex is the index of the child that has been removed.
        \sa AccessibleObject::child(), childAdded()
     */
    void childRemoved(const QAccessibleClient::AccessibleObject &parent, int childIndex);
    void visibleDataChanged(const QAccessibleClient::AccessibleObject &object);
    void selectionChanged(const QAccessibleClient::AccessibleObject &object);
    void modelChanged(const QAccessibleClient::AccessibleObject &object);

    /**
        \brief Emitted when the focus changed.

        When subscribed to the Focus EventListener then this signal is emitted
        every time the focus changes. \a object is the newly focused AccessibleObject.
    */
    void focusChanged(const QAccessibleClient::AccessibleObject &object);

    /**
        \brief Emitted when the text cared moved.

        When subscribed to the TextCaretMoved EventListener then this signal
        is emitted every time the caret in an accessible object that implements
        a text-interface (like QLineEdit, QTextArea and QComboBox) moved to
        another position.
    */
    void textCaretMoved(const QAccessibleClient::AccessibleObject &object, int pos);

    /**
        \brief Emitted when the text selection changed.

        When subscribed to the TextSelectionChanged EventListener then this signal
        is emitted every time the selection in an accessible object that implements
        a text-interface (like QLineEdit, QTextArea and QComboBox) changed.
    */
    void textSelectionChanged(const QAccessibleClient::AccessibleObject &object);


    /**
        \brief Emitted when an accessible name changes

        When subscribed to the PropertyChanged EventListener, this signal is emitted
	whenever the accessible-name property changes its value.
    */
    void accessibleNameChanged(const QAccessibleClient::AccessibleObject &object);

    /**
        \brief Emitted when an accessible description changes

        When subscribed to the PropertyChanged EventListener, this signal is emitted
	whenever the accessiblei-description property changes its value.
    */
    void accessibleDescriptionChanged(const QAccessibleClient::AccessibleObject &object);

    //void textBoundsChanged(const QAccessibleClient::AccessibleObject &object);
    void textChanged(const QAccessibleClient::AccessibleObject &object);
    //void textAttributesChanged(const QAccessibleClient::AccessibleObject &object);
    //void attributesChanged(const QAccessibleClient::AccessibleObject &object);

private:
    Q_DISABLE_COPY(Registry)
    RegistryPrivate *d;
    friend class RegistryPrivate;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Registry::EventListeners)

}

#endif
