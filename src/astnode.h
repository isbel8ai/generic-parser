/**
* @file
* @author Isbel Ochoa Izquierdo
*/

#ifndef ASTNODE_H
#define ASTNODE_H

#include <QString>
#include <QStringRef>
#include <QList>
#include <QDomDocument>

#define ATTR_NAME "name"
#define ATTR_POS "position"
#define ATTR_LENGTH "length"

/**
* AstNode representa un nodo del árbol que se genera un analizador de texto.
*/
class AstNode
{
    protected:

        /** Etiqueta de la estructura sintáctica identificada. */
        QString tagName;

        /**
        * Nombre que toma la vriable definida para la referencia de texto al
        * ser identificada en la sintaxis.
        */
        QString name;

        /** Referencia al texto en la entrada de texto analizada. */
        QStringRef textReference;

        /** Lista de punteros a los nodos hijos del nodo en cuestión. */
        QList<AstNode *> childList;

    public:

        /** Constructor por defecto. */
        AstNode();

        /**
        * Constructor de inicialización de atributos.
        * @param tag etiqueta de la estructura identificada.
        * @param txtRef referencia de texto del nodo.
        * @param var nombre de variable definida para el nodo.
        */
        AstNode(QString tag, QStringRef txtRef, QString var = QString());

        /** Destructor. */
        ~AstNode();

        /** Elimina todos los hijos de este nodo. */
        void clear();

        /**
        * Retorna si el nodo es nulo.
        * @return Devuelve true si la referencia de texto es vacía, de lo
        * contrario devuele false.
        */
        bool isNull();

        /** Retorna la etiqueta de sintaxis identificada. */
        QString getTagName();

        /** Retorna el nombre la variable indentificada. */
        QString getName();

        /**
        * Establece el nombre de la variable identificada.
        * @param name valor a establecer en el atributo name de la clase.
        */
        void setName(QString name);

        /** Retorna la referencia al texto de este nodo. */
        QStringRef getReference();

        /** Retorna la cantidad de nodos hijos de este nodo. */
        int childCount();

        /**
        * Adiciona un nodo a la lista de hijos.
        * @param child puntero al nodo que se agregará como hijo.
        */
        void addChild(AstNode *child);

        /** Obtiene el texto referenciado por textReferece. */
        QString toString();

        /**
        * Convierte el árbol en un objeto QDomElement que es agregado al
        * documento xml.
        * @param xml documento al que se agregará la estructura del árbol.
        * @return retorna el objeto QDomDocument que ha sido agregado a xml.
        */
        QDomElement toDom(QDomDocument *xml);
};

#endif
