/**
 * @file
 * @author Isbel Ochoa Izquierdo
 */

#include <QDebug>
#include <QFile>

#include <parser.h>
#include <astnode.h>
#include <dictionarymanager.h>

Parser::Parser(QDomElement rules, DictionaryManager *dictMgr) {
    this->rules = rules;
    this->format = rules.attribute(ATTR_NAME, DEFAULT_FORMAT);
    this->dictManager = dictMgr;
}

QString Parser::getFormat() {
    return this->format;
}

void Parser::setRules(QDomElement rules) {
    this->rules = rules;
    this->format = rules.attribute(ATTR_NAME, DEFAULT_FORMAT);
}

QRegExp Parser::matchExp() {

    /* Se busca la primera producción de la gramática.*/
    QDomElement start = rules.firstChildElement(format);
    if (start.isNull()) {
        qCritical() << "Parser: No se pudo encontrar el elemento inicial de "
                       "la gramatica del formato " << format;
        return QRegExp();
    }

    /* Se obtiene el texto de la expresión regular de la primera producción.*/
    QString regexpStr = start.attribute(ATTR_REGEXP);
    if (regexpStr.isEmpty()) {
        qCritical() << "Parser: No se ha definido el atributo " ATTR_REGEXP
                       " para el elemento" << format;
        return QRegExp();
    }

    /* Se genera la expresión regular de la primera producción.*/
    QRegExp regexp(regexpStr);
    if (!regexp.isValid()) {
        qCritical() << "Parser: La expresion regular para el elemento" <<
                       format << "no es correcta";
        return QRegExp();
    }

    regexp.setMinimal(true);
    return regexp;
}

AstNode *Parser::parse(QString *input) {

    /* Se busca la primera producción de la gramática.*/
    QDomElement start = rules.firstChildElement(format);
    if (start.isNull()) {
        qCritical() << "No se pudo encontrar el elemento inicial de la "
                       "gramatica del formato" << format;
        return NULL;
    }

    /* Se procesa la entrada de texto en busca de una aparición del formato
    desado.*/
    QStringRef matchRef(input);
    AstNode *block = process(matchRef, start);

    /* Si no coincide el texto analizado con la expresión regular.*/
    if (!block) {
        return NULL;
    }

    if (block->isNull()) {
        delete block;
        return NULL;
    }

    return block;
}

AstNode *Parser::process(QStringRef &textRef, QDomElement subRules) {

    /* Se obtienen los atributos de la referencia de texto a analizar.*/
    const QString *text = textRef.string();
    int startPos = textRef.position();
    int length = textRef.length();

    /* Se obtienen los valores asociados a la reglas sintácticas.*/
    QString tagName = subRules.tagName();
    QString varName = subRules.attribute(ATTR_NAME);
    QString className = subRules.attribute(ATTR_CLASS);
    bool required = subRules.attribute(ATTR_REQUIRED) != REQUIRED_FALSE;

    /* Si la entrada de texto esta vacía.*/
    if (length == 0) {
        return required ? NULL : new AstNode();
    }

    /* Si el atributo class no esta definido en las reglas.*/
    if (className.isEmpty()) {
        qCritical() << "Parser: No se ha definido el atributo " ATTR_CLASS
                       " para el elemento" << tagName;
        return NULL;
    }

    AstNode *result = NULL;

    /* Si, según las reglas, se espera encontrar un elemento terminal de la
    gramática.*/
    if (className == CLASS_REG_TERMINAL || className == CLASS_DIC_TERMINAL) {

        QRegExp regexp;

        /* Si se espera encontrar un elemento terminal definido por una
        expresión regurlar.*/
        if (className == CLASS_REG_TERMINAL) {
            QString regexpStr = subRules.attribute(ATTR_REGEXP);
            if (regexpStr.isEmpty()) {
                qCritical() << "Parser: No se ha definido el atributo "
                               ATTR_REGEXP " para el elemento" << tagName;
                return required ? NULL : new AstNode();
            }
            regexp = QRegExp(regexpStr);

            regexp.setMinimal(true);

            /* Si se espera encontrar un elemento terminal definido en
        diccionario.*/
        } else {
            regexp = dictManager->getDictionary(tagName);
        }

        /* Se comprueba la validez de la expresión regular.*/
        if (!regexp.isValid() || regexp.isEmpty()) {
            qCritical() << "Parser: La expresion regular para el elemento" <<
                           tagName << "no es correcta";
            return required ? NULL : new AstNode();
        }

        int pos = textRef.toString().indexOf(regexp);

        /* Si no coincide el texto analizado con la expresión regular.*/
        if (pos == -1) {
            return required ? NULL : new AstNode();
        }

        int count = regexp.matchedLength();
        QStringRef tmpRef(text, startPos + pos, count);
        result = new AstNode(tagName, tmpRef, varName);

        /* Se adelanta la referencia de texto hasta la posición siguiente al
        texto reconocido.*/
        int offset = pos + count;
        textRef = QStringRef(text, startPos + offset, length - offset);

        /* Si, según las reglas, se espera encontrar un elemento no terminal de la
    gramática.*/
    } else if (className == CLASS_INITIAL || className == CLASS_NON_TERMINAL) {

        QStringRef tmpRef = textRef;

        /* Se obtienen el texto de la expresión regular.*/
        QString regexpStr = subRules.attribute(ATTR_REGEXP);
        if (regexpStr.isEmpty()) {
            qCritical() << "Parser: No se ha definido el atributo " ATTR_REGEXP
                           " para el elemento" << tagName;
        }

        /* Se genera la expresión regular.*/
        QRegExp regexp = QRegExp(regexpStr);
        if (!regexp.isValid()) {
            qCritical() << "Parser: La expresion regular para el elemento " <<
                           tagName << "no es correcta";
            return required ? NULL : new AstNode();
        }
        regexp.setMinimal(true);

        int pos = textRef.toString().indexOf(regexp);

        /* Si no coincide el texto analizado con la expresión regular.*/
        if (pos == -1) {
            return required ? NULL : new AstNode();
        }

        /* Se adelanta la referencia de texto hasta la posición siguiente al
        texto reconocido.*/
        int count = regexp.matchedLength();
        tmpRef = QStringRef(text, startPos + pos, count);

        /* Si se analiza la primera producción de la gramática se establese como
        nombre del tag "output".*/
        if (tagName == format) {
            result = new AstNode("output", tmpRef);
        } else {
            result = new AstNode(tagName, tmpRef, varName);
        }

        QDomElement cursor = subRules.firstChildElement();

        if (cursor.isNull()) {
            textRef = QStringRef(text, tmpRef.position() + tmpRef.length(),
                                 length - tmpRef.length() - tmpRef.position() + startPos);
            return result;
        }

        /* Se analiza el texto con según las reglas definidas para cada
        derivación de la regla*/
        while (!cursor.isNull()) {
            AstNode *part = process(tmpRef, cursor);
            if (!part) {
                delete result;
                return required ? NULL : new AstNode();
            }
            if (part->isNull()) {
                delete part;
            } else {
                result->addChild(part);
            }
            cursor = cursor.nextSiblingElement();
        }

        /* Se adelanta la referencia de texto hasta la posición siguiente al
        texto reconocido.*/
        int newPos = tmpRef.position();
        int newLength = length - newPos + startPos;
        textRef = QStringRef(text, newPos, newLength);

        /* Si, según las reglas, se espera encontrar una lista o colección de
    elementos gramaticales.*/
    } else if (className == CLASS_LIST || className == CLASS_COLLECTION) {

        QStringRef tmpRef = textRef;

        /* Se comprueba la existencia de un hijo para la regla.*/
        QDomElement elem = subRules.firstChildElement();
        if (elem.isNull()) {
            qCritical() << "Parser: No existen hijos para el elemento" <<
                           tagName;
            return required ? NULL : new AstNode();
        }

        /* Se crea el nodo que se retornará.*/
        result = new AstNode(tagName, tmpRef, varName);

        /* Si se analiza una lista.*/
        if (className == CLASS_LIST) {
            AstNode *part = process(tmpRef, elem);

            /* Se identifican todos los elementos a listar y se agregan como
            hijos del nodo result.*/
            while (part) {
                if (part->isNull()) {
                    delete part;
                    part = NULL;
                } else {
                    result->addChild(part);
                    part = process(tmpRef, elem);
                }
            }
            /* Si se analiza un conjunto de elementos desordenados.*/
        } else {

            /* Se identifican todos los elementos a listar por cada categoria
            dentro del conjunto y se agregan como hijos del nodo result.*/
            while (!elem.isNull()) {
                QStringRef localRef = textRef;
                AstNode *part = process(localRef, elem);
                while (part) {
                    if (part->isNull()) {
                        delete part;
                        part = NULL;
                    } else {
                        result->addChild(part);
                        part = process(localRef, elem);
                    }
                }
                if (localRef.position() > tmpRef.position()) {
                    tmpRef = localRef;
                }
                elem = elem.nextSiblingElement();
            }
        }

        /* Se comprueba si se ha identificado algún elemento de la lista o
        conjunto.*/
        if (result->childCount() == 0) {
            delete result;
            return required ? NULL : new AstNode();
        }

        /* Se adelanta la referencia de texto hasta la posición siguiente al
        texto reconocido.*/
        int newPos = tmpRef.position();
        int newLength = length - newPos + startPos;
        textRef = QStringRef(text, newPos, newLength);

        /* Si la regla que se ha de procesar es una referencia.*/
    } else if (className == CLASS_REFERENCE) {

        /* Se comprueba la existencia de alguna producción con igual tag que la
        referencia.*/
        QDomElement cursor = this->rules.firstChildElement(tagName);
        if (cursor.isNull()) {
            qCritical() << "Parser: No se pudo encontrar el elemento" <<
                           tagName;
            return required ? NULL : new AstNode();
        }

        bool matched = false;
        QStringRef tmpRef = textRef;

        /* Se anliza el texto con cada una de las producciones que tienen el
        mismo tag que la referencia, hasta encontrar una que coincida.*/
        while (!cursor.isNull() && !matched) {
            result = process(tmpRef, cursor);
            if (result) {
                if (result->isNull()) {
                    delete result;
                    result = NULL;
                } else {
                    matched = true;
                    if (!varName.isEmpty()) {
                        result->setName(varName);
                    }
                }
            }
            cursor = cursor.nextSiblingElement(tagName);
        }

        /* Se comprueba si hubo alguna coincidencia.*/
        if (!matched) {
            return required ? NULL : new AstNode();
        }

        /* Se adelanta la referencia de texto hasta la posición siguiente al
        texto reconocido.*/
        textRef = tmpRef;

        /* Si la regla que se ha de procesar es una lista de opciones.*/
    } else if (className == CLASS_OPTION) {

        /* Se comprueba la existencia de un hijo para la regla.*/
        QDomElement cursor = subRules.firstChildElement();
        if (cursor.isNull()) {
            qCritical() << "Parser: No existen hijos para el elemento" <<
                           tagName;
            return required ? NULL : new AstNode();
        }

        int pos = startPos + length;
        int len = 0;

        /* Se comprueba cual de las opciones esperadas se ecuentra más próxima
        al inicio del texto analizado.*/
        while (!cursor.isNull()) {
            QStringRef tmpRef = textRef;
            AstNode * option = process(tmpRef, cursor);
            if (option) {
                if (option->isNull()) {
                    delete option;
                } else if ((option->getReference().position() < pos) ||
                           ((option->getReference().position() == pos) &&
                            (option->getReference().length() > len))){

                    if (result) {
                        delete result;
                    }

                    result = option;
                    pos = option->getReference().position();
                    len = option->getReference().length();
                }
            }
            cursor = cursor.nextSiblingElement();
        }

        /* Si pos no se modifica significa que no se encontró ninguna
        coincidencia.*/
        if (pos == startPos + length) {
            return required ? NULL : new AstNode();
        }

        /* Se establese un nombre de variable si ha sido definido.*/
        if (!varName.isEmpty()) {
            result->setName(varName);
        }

        /* Se adelanta la referencia de texto hasta la posición siguiente al
        texto reconocido.*/
        int newPos = pos + result->getReference().length();
        int newLength = length - newPos + startPos;
        textRef = QStringRef(text, newPos, newLength);

        /* Si no se identifica la clase de la regla.*/
    } else {
        qCritical() << "Parser: No se pudo reconocer la clase" << className;
    }
    return result;
}

