/**
* @file
* @author Isbel Ochoa Izquierdo
*/

#ifndef PARSERMANAGER_H
#define PARSERMANAGER_H

#include <QString>
#include <QList>
#include <QByteArray>
#include <QDir>
#include <QDomDocument>

class DictionaryManager;
class Parser;

/**
* ParserManager cumple la función de gestionar los analizadores de texto para
* cada formato.
*/
class ParserManager
{
    private:

        /** Directorio de archivos de configuración. */
        QDir configDirectory;

        /** Gestor de diccionarios. */
        DictionaryManager *dictionaries;

        /** Lista de analizadores de texto. */
        QList<Parser *> parserList;

    public:

        /**
        * Constructor.
        * @param confDir directorio de búsqueda de archivos de configuración.
        */
        ParserManager(QDir confDir);

        /** Destructor. */
        ~ParserManager();

        QDomElement loadConfiguration(QString format);

        /**
        * Busca un analizador en la lista de analizadores sintácticos.
        * @param format formato del analizador buscado.
        * @return Devuelve la posición del analizador si se encuentra en la
        * lista, de lo contrario retorna -1.
        */
        int findParser(QString format);

        /** Carga la configuración del parser para el formato format. */
        bool configureParser(QString format);

        /**
        * Analiza la entrada de texto apuntada por input con el parser psr.
        * @param imput apunta a la entrada de texto que se analizará.
        * @param doc documento DOM donde se crea el elemento a devolver.
        * @param psr parser con el cual se procesará la entrada.
        * @return Devuelve la cantidad de ocurrencias del formato identificado
        * por el analizador de índice parserPos encontradas en la entrada.
        */
        int parseFormat(QString *input, QDomDocument &doc, int parserPos);

        /**
        * Analiza la entrada de texto apuntada por input con el parser cuyo
        * formato es frmt.
        * @param imput apunta a la entrada de texto que se analizará.
        * @param doc documento DOM donde se crea el elemento a devolver.
        * @param frmt formato del parser con el cual se procesará la entrada.
        * @return Devuelve la cantidad de ocurrencias del formato format
        * encontradas en la entrada. Retorna -1 si no exite un analizador para
        * dicho formato
        */
        int parseFormat(QString *input, QDomDocument &doc, QString format);

        QDomDocument parseAll(QString *input);

        /**
        * Analiza la entrada de texto apuntada por input con todos los
        * analizadores de texto disponibles en parserList. Retorna un documento
        * DOM que contiene la estructura xml resultante del proceso.
        * @param imput apunta a la entrada de texto que se analizará.
        * @return Devuelve el documento DOM resultante del proceso de análisis
        * del texto.
        */
        QDomDocument toDom(QString *input);

        /**
        * Analiza la entrada de texto apuntada por input con todos los
        * analizadores de texto disponibles en parserList. Retorna un arreglo
        * de bytes que contiene el xml resultante del proceso.
        * @param imput apunta a la entrada de texto que se analizará.
        * @return Devuelve el xml resultante del proceso de análisis del texto.
        */
        QByteArray toXml(QString *input);

        /** Carga el contenido del diccionario de nombre key. */
        bool loadDictionary(QString key);
};

#endif // PARSERCONTROLLER_H
