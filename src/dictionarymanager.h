/**
* @file
* @author Isbel Ochoa Izquierdo
*/

#ifndef DICTIONARYMANAGER_H
#define DICTIONARYMANAGER_H

#include <QDir>
#include <QHash>

/**
* DictionaryManager es la clase encargada de gestionar el trabajo con los
* diccionarios utilizados por la clase Parser.
*/
class DictionaryManager
{
    private:

        /** Directorio donde se buscan los diccionarios. */
        QDir directory;

        /**
        * Tabla hash donde se guardan las expresiones regulares para cada
        * elemnto gramatical definido por diccionario.
        */
        QHash<QString, QRegExp> dictionaries;

    public:

        /**
        * Constructor.
        * @param dir directorio de búsqueda de diccionarios.
        */
        DictionaryManager(QDir dir);

        /**
        * Retorna la expresion regular del diccionario del elemento gramatical
        * de nombre key. Si no se encuentra el diccionario entrega una expresón
        * regular vacía.
        * @param key nombre del diccionario.
        * @return Devuelve la expresión regular para el diccionario key.
        * @see loadDictionary
        */
        QRegExp getDictionary(QString key);

        /**
        * Carga el diccionario de nombre key desde un fichero de igual nombre y
        * luego retorna la expresión regular correspondiente. Si no se puede
        * formar correctamente la expresión regular, una expresón regular vacía
        * es retornada.
        * @param key nombre del diccionario.
        * @return Devuelve la expresión regular para el diccionario key.
        * @see getDictionary
        */
        QRegExp loadDictionary(QString key);

};

#endif // DICTIONARYMANAGER_H
