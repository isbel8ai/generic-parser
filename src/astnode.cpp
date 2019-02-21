/**
 * @file
 * @author Isbel Ochoa Izquierdo
 */

#include <astnode.h>

AstNode::AstNode() {
    this->tagName = QString();
    this->name = QString();
    this->textReference = QStringRef();
}

AstNode::AstNode(QString tag, QStringRef txtRef, QString var) {
    this->tagName = tag;
    this->textReference = txtRef;
    this->name = var.isEmpty() ? tag : var;
}

AstNode::~AstNode() {
    clear();
}

void AstNode::clear() {
    name.clear();
    textReference.clear();
    for (int i = 0; i < childList.size(); ++i) {
        if (childList[i]) {
            delete childList[i];
        }
    }
    childList.clear();
}

bool AstNode::isNull() {
    return textReference.isEmpty();
}

QString AstNode::getTagName() {
    return this->tagName;
}

QString AstNode::getName() {
    return this->name;
}

void AstNode::setName(QString name) {
    this->name = name;
}

QStringRef AstNode::getReference() {
    return this->textReference;
}

int AstNode::childCount() {
    return childList.size();
}

void AstNode::addChild(AstNode *child) {

    /* Si el puntero es NULL no es agregado a la lista de hijos.*/
    if (!child) {
        return;
    }

    if (child->isNull()) {
        delete child;
        child = NULL;
        return;
    }

    /* Si la lista de elementos hijos está vacía se agrega el puntero.*/
    if (childList.isEmpty()) {
        childList.append(child);
        return;
    }

    /* Se obtienen las posiciones del último elemento de la lista y la del
    elemento que se debe agregar.*/
    int lastPos = childList.last()->getReference().position();
    int childPos = child->getReference().position();

    /* Si el nuevo elemento está posicionado después del último elemento de la
    lista se adiciona al final*/
    if (childPos > lastPos) {
        childList.append(child);

        /* Si el nuevo elemento está posicionado antes del último elemento de la
    lista se procede a buscar la posición donde debe ser insertado.*/
    } else {
        int begin = 0;
        int end = childList.size() - 1;
        while (begin < end -1) {
            /*búsqueda binaria de la posición en la que se va a insertar el
            nuevo elemento.*/
            int pivot = (begin + end) / 2;
            int testPos = childList.at(pivot)->getReference().position();
            if (childPos > testPos) {
                begin = pivot;
            } else {
                end = pivot;
            }
        }
        childList.insert(end, child);
    }
}

QString AstNode::toString() {
    return textReference.toString();
}

QDomElement AstNode::toDom(QDomDocument *xml) {

    /* Se crea el elemento que se va a retornar.*/
    QDomElement result = xml->createElement(tagName);

    /* Se le agregan al elemento los atributos de posición y longitud del texto
    al que hace referencia.*/
    result.setAttribute(ATTR_POS, textReference.position());
    result.setAttribute(ATTR_LENGTH, textReference.length());

    /* Se agrega un atrubuto 'name' si el nombre definido es diferente al nombre
    del tag.*/
    if (name != tagName) {
        result.setAttribute(ATTR_NAME, name);
    }

    /* Si el nodo no tiene elementos hijos se agrega el texto referenciado.*/
    if (childCount() == 0) {
        result.appendChild(xml->createTextNode(textReference.toString()));
        return result;
    }

    /* Si el nodo tiene elementos hijos se agregan los elementos resultantes de
    procesar cada uno de ellos.*/
    for (int i = 0; i < childCount(); ++i) {
        result.appendChild(childList.at(i)->toDom(xml));
    }

    return result;
}
