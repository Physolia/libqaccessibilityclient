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

#include "accessibleobject.h"

#include <qstring.h>
#include <qdebug.h>

#include "accessibleobject_p.h"
#include "registry_p.h"

#include <atspi/atspi-constants.h>

using namespace QAccessibleClient;

AccessibleObject::AccessibleObject()
    :d(0)
{
}

AccessibleObject::AccessibleObject(RegistryPrivate *registryPrivate, const QString &service, const QString &path)
    :d(0)
{
    Q_ASSERT(registryPrivate);
    Q_ASSERT(!service.isEmpty());
    Q_ASSERT(!path.isEmpty());
    if (registryPrivate->m_cacheStrategy) {
        const QString id = path + service;
        d = registryPrivate->m_cacheStrategy->get(id);
        if (!d) {
            d = QSharedPointer<AccessibleObjectPrivate>(new AccessibleObjectPrivate(registryPrivate, service, path));
            registryPrivate->m_cacheStrategy->add(id, d);
        }
    } else {
        d = QSharedPointer<AccessibleObjectPrivate>(new AccessibleObjectPrivate(registryPrivate, service, path));
    }
}

AccessibleObject::AccessibleObject(const QSharedPointer<AccessibleObjectPrivate> &dd)
    :d(dd)
{
}

AccessibleObject::AccessibleObject(const AccessibleObject &other)
    : d(other.d)
{
}

AccessibleObject::~AccessibleObject()
{
}

QString AccessibleObject::id() const
{
    if (!d || !d->registryPrivate)
        return QString();
    return d->path + d->service;
}

QUrl AccessibleObject::url() const
{
    if (!d || !d->registryPrivate)
        return QUrl();
    QUrl u;
    u.setScheme(d->registryPrivate->ACCESSIBLE_OBJECT_SCHEME_STRING);
    u.setPath(d->path);
    u.setFragment(d->service);
    return u;
}

bool AccessibleObject::isValid() const
{
    return d && d->registryPrivate
             && (!d->service.isEmpty())
             && (!d->path.isEmpty())
             && (d->path != QLatin1String("/org/a11y/atspi/null"));
}

AccessibleObject &AccessibleObject::operator=(const AccessibleObject &other)
{
    d = other.d;
    return *this;
}

bool AccessibleObject::operator==(const AccessibleObject &other) const
{
    return (d == other.d) || (d && other.d && *d == *other.d);
}

AccessibleObject AccessibleObject::parent() const
{
    return d->registryPrivate->parentAccessible(*this);
}

QList<AccessibleObject> AccessibleObject::children() const
{
    return d->registryPrivate->children(*this);
}

QVector< QList<AccessibleObject> > AccessibleObject::children(const QList<Role> &roles) const
{
    QVector< QList<AccessibleObject> > result(roles.count());
    QList<AccessibleObject> all = children();
    for(int i = 0; i < all.count(); ++i) {
        const AccessibleObject &child = all[i];
        int index = roles.indexOf(child.role());
        if (index < 0) continue;
        result[index].append(child);
    }
    return result;
}

int AccessibleObject::childCount() const
{
    return d->registryPrivate->childCount(*this);
}

AccessibleObject AccessibleObject::child(int index) const
{
    return d->registryPrivate->child(*this, index);
}

int AccessibleObject::indexInParent() const
{
    return d->registryPrivate->indexInParent(*this);
}

QString AccessibleObject::name() const
{
    return d->registryPrivate->name(*this);
}

QString AccessibleObject::description() const
{
    return d->registryPrivate->description(*this);
}

AccessibleObject::Role AccessibleObject::role() const
{
    return d->registryPrivate->role(*this);
}

QString AccessibleObject::roleName() const
{
    return d->registryPrivate->roleName(*this);
}

QString AccessibleObject::localizedRoleName() const
{
    return d->registryPrivate->localizedRoleName(*this);
}

int AccessibleObject::layer() const
{
    return d->registryPrivate->layer(*this);
}

int AccessibleObject::mdiZOrder() const
{
    return d->registryPrivate->mdiZOrder(*this);
}

double AccessibleObject::alpha() const
{
    return d->registryPrivate->alpha(*this);
}

QRect AccessibleObject::boundingRect() const
{
    if( supportedInterfaces() & AccessibleObject::ComponentInterface ){
        return d->registryPrivate->boundingRect(*this);
    } else {
        qWarning() << "boundingRect called on accessible that does not implement component";
        return QRect();
    }
}

QRect AccessibleObject::characterRect(int offset) const
{
    if( supportedInterfaces() & AccessibleObject::TextInterface ){
        return d->registryPrivate->characterRect(*this, offset);
    } else {
        qWarning() << "characterRect called on accessible that does not implement text";
        return QRect();
    }
}

AccessibleObject::Interfaces AccessibleObject::supportedInterfaces() const
{
    return d->registryPrivate->supportedInterfaces(*this);
}

int AccessibleObject::caretOffset() const
{
    if( supportedInterfaces() & AccessibleObject::TextInterface ){
        return d->registryPrivate->caretOffset(*this);
    } else {
        qWarning() << "caretOffset called on accessible that does not implement text";
        return 0;
    }
}

int AccessibleObject::characterCount() const
{
    if( supportedInterfaces() & AccessibleObject::TextInterface ){
        return d->registryPrivate->characterCount(*this);
    } else {
        qWarning() << "characterCount called on accessible that does not implement text";
        return 0;
    }
}

QString AccessibleObject::text(int startOffset, int endOffset) const
{
    if( supportedInterfaces() & AccessibleObject::TextInterface )
        return d->registryPrivate->text(*this, startOffset, endOffset);
    qWarning() << "text called on accessible that does not implement text";
    return QString();
}

bool AccessibleObject::setText(const QString &text)
{
    if( supportedInterfaces() & AccessibleObject::EditableTextInterface )
        return d->registryPrivate->setText(*this, text);
    qWarning() << "setText called on accessible that does not implement editableText";
    return false;
}

bool AccessibleObject::insertText(const QString &text, int position, int length)
{
    if( supportedInterfaces() & AccessibleObject::EditableTextInterface )
        return d->registryPrivate->insertText(*this, text, position, length);
    qWarning() << "insertText called on accessible that does not implement editableText";
    return false;
}

bool AccessibleObject::copyText(int startPos, int endPos)
{
    if( supportedInterfaces() & AccessibleObject::EditableTextInterface )
        return d->registryPrivate->copyText(*this, startPos, endPos);
    qWarning() << "copyText called on accessible that does not implement editableText";
    return false;
}

bool AccessibleObject::cutText(int startPos, int endPos)
{
    if( supportedInterfaces() & AccessibleObject::EditableTextInterface )
        return d->registryPrivate->cutText(*this, startPos, endPos);
    qWarning() << "cutText called on accessible that does not implement editableText";
    return false;
}

bool AccessibleObject::deleteText(int startPos, int endPos)
{
    if( supportedInterfaces() & AccessibleObject::EditableTextInterface )
        return d->registryPrivate->deleteText(*this, startPos, endPos);
    qWarning() << "deleteText called on accessible that does not implement editableText";
    return false;
}

bool AccessibleObject::pasteText(int position)
{
    if( supportedInterfaces() & AccessibleObject::EditableTextInterface )
        return d->registryPrivate->pasteText(*this, position);
    qWarning() << "pasteText called on accessible that does not implement editableText";
    return false;
}

QList< QPair<int,int> > AccessibleObject::textSelections() const
{
    if( supportedInterfaces() & AccessibleObject::Text )
        return d->registryPrivate->textSelections(*this);
    qWarning() << "textSelections called on accessible that does not implement text";
    return QList< QPair<int,int> >();
}

void AccessibleObject::setTextSelections(const QList< QPair<int,int> > &selections)
{
    if( supportedInterfaces() & AccessibleObject::Text )
        return d->registryPrivate->setTextSelections(*this, selections);
    qWarning() << "setTextSelections called on accessible that does not implement text";
}

QPoint AccessibleObject::focusPoint() const
{
    Interfaces ifaces = supportedInterfaces();
    if (ifaces & TextInterface) {
        int offset = caretOffset();
        QRect r = characterRect(offset);
        if (r.x() != 0 || r.y() != 0)
            return r.center();
    }
    if (ifaces & ComponentInterface) {
        QRect r = boundingRect();
        if (!r.isNull())
            return r.center();
    }
    AccessibleObject p = parent();
    if (p.isValid())
        return p.focusPoint(); // recursive
    return QPoint();
}

AccessibleObject AccessibleObject::application() const
{
    return d->registryPrivate->application(*this);
}

QString AccessibleObject::appToolkitName() const
{
    return d->registryPrivate->appToolkitName(*this);
}

QString AccessibleObject::appVersion() const
{
    return d->registryPrivate->appVersion(*this);
}

int AccessibleObject::appId() const
{
    return d->registryPrivate->appId(*this);
}

QString AccessibleObject::appLocale(LocaleType lctype) const
{
    return d->registryPrivate->appLocale(*this, lctype);
}

QString AccessibleObject::appBusAddress() const
{
    return d->registryPrivate->appBusAddress(*this);
}

double AccessibleObject::minimumValue() const
{
    return d->registryPrivate->minimumValue(*this);
}

double AccessibleObject::maximumValue() const
{
    return d->registryPrivate->maximumValue(*this);
}

double AccessibleObject::minimumValueIncrement() const
{
    return d->registryPrivate->minimumValueIncrement(*this);
}

double AccessibleObject::currentValue() const
{
    return d->registryPrivate->currentValue(*this);
}

QList<AccessibleObject> AccessibleObject::selection() const
{
    return d->registryPrivate->selection(*this);
}

QString AccessibleObject::imageDescription() const
{
    return d->registryPrivate->imageDescription(*this);
}

QString AccessibleObject::imageLocale() const
{
    return d->registryPrivate->imageLocale(*this);
}

QRect AccessibleObject::imageRect() const
{
    return d->registryPrivate->imageRect(*this);
}

QVector< QSharedPointer<QAction> > AccessibleObject::actions() const
{
    // Actions in atspi are supposed to be static what means they cannot change in
    // between (e.g. actions removed or added or edited) so we can safely just
    // fetch them only once and store the result for the life-time of the object,
    if (!d->actionsFetched) {
        d->actionsFetched = true;
        d->actions = d->registryPrivate->actions(*this);
    }
    return d->actions;
}

bool AccessibleObject::hasSelectableText() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_SELECTABLE_TEXT);
}

bool AccessibleObject::hasToolTip() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_HAS_TOOLTIP);
}

bool AccessibleObject::isActive() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_ACTIVE);
}

bool AccessibleObject::isCheckable() const
{
    //FIXME: Find better AccessibleObject::isCheckable
    //return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_);

    Role role = d->registryPrivate->role(*this);
    if (role == AccessibleObject::CheckBox ||
        role == AccessibleObject::CheckableMenuItem ||
        role == AccessibleObject::RadioButton ||
        role == AccessibleObject::RadioMenuItem ||
        role == AccessibleObject::ToggleButton)
            return true;
    return false;
}

bool AccessibleObject::isChecked() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_CHECKED);
}

bool AccessibleObject::isDefunct() const
{
    return d->defunct;
}

bool AccessibleObject::isDefault() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_IS_DEFAULT);
}

bool AccessibleObject::isEditable() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_EDITABLE);
}

bool AccessibleObject::isEnabled() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_ENABLED);
}

bool AccessibleObject::isExpandable() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_EXPANDABLE);
}

bool AccessibleObject::isExpanded() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_EXPANDED);
}

bool AccessibleObject::isFocusable() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_FOCUSABLE);
}

bool AccessibleObject::isFocused() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_FOCUSED);
}

bool AccessibleObject::isMultiLine() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_MULTI_LINE);
}

bool AccessibleObject::isSelectable() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_SELECTABLE);
}

bool AccessibleObject::isSelected() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_SELECTED);
}

bool AccessibleObject::isSensitive() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_SENSITIVE);
}

bool AccessibleObject::isSingleLine() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_SINGLE_LINE);
}

bool AccessibleObject::isVisible() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_VISIBLE);
}

bool AccessibleObject::supportsAutocompletion() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_SUPPORTS_AUTOCOMPLETION);
}

#ifndef QT_NO_DEBUG_STREAM
QACCESSIBILITYCLIENT_EXPORT QDebug QAccessibleClient::operator<<(QDebug d, const AccessibleObject &object)
{
    d.nospace();
    d << "AccessibleObject("; //d:" << hex << (void *) object.d << dec;
    if (object.d) {
        d << "service=" << object.d->service;
        d << " path=" << object.d->path;
    } else {
        d << "invalid";
    }
    d << ")";
    return d.space();
}
#endif

uint qHash(const QAccessibleClient::AccessibleObject& object) {
    return qHash(object.d);
}
