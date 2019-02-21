/**
* @file
* @author Isbel Ochoa Izquierdo
*/

#ifndef PARSER_H
#define PARSER_H

#include <QDomElement>
#include <QRegExp>
#include <QHash>
#include <QDir>

#define DEFAULT_FORMAT "default"

#define ATTR_CLASS "class"
#define ATTR_REGEXP "regexp"
#define ATTR_NAME "name"
#define ATTR_TYPE "type"
#define ATTR_REQUIRED "required"

#define CLASS_INITIAL "initial"
#define CLASS_REFERENCE "reference"
#define CLASS_NON_TERMINAL "non_terminal"
#define CLASS_REG_TERMINAL "reg_terminal"
#define CLASS_DIC_TERMINAL "dic_terminal"
#define CLASS_OPTION "option"
#define CLASS_LIST "list"
#define CLASS_COLLECTION "collection"

#define REQUIRED_FALSE "false"
#define REQUIRED_TRUE "true"

class DictionaryManager;
class AstNode;

/**
* Parser es la clase encargada de analizar textos basándose en la sintaxis
* definida para el formato específico que procesa.
*/
class Parser
{
    private:

        /** Nombre del formato que analiza este parser */
        QString format;

        /** Reglas sintácticas del formato */
        QDomElement rules;

        /** Gestor de diccionarios */
        DictionaryManager *dictManager;

    public:

        /**
        * Constructor.
        * @param rules reglas sintácticas del parser.
        * @param dictMgr puntero al gestor de dicionarios.
        */
        Parser(QDomElement rules, DictionaryManager *dictMgr);

        /** Retorna el nombre del formato de este parser */
        QString getFormat();

        /** Establece las reglas sintácticas para el formato del parser. */
        void setRules(QDomElement rules);

        /** Retorna la expreción regular que identifica el formato del parser */
        QRegExp matchExp();        

        /**
        * Analiza una entrada de texto y retorna un árbol sintácticamente
        * organizado según el formato a identificar. Si la sintaxis de la
        * entrada no es correcta devuelve NULL.
        * @param input puntero a la entrada de texto.
        * @return Devuelve el árbol que representa la estructrua sintáctica
        * reconocida, o NULL si no se reconoce.
        */
        AstNode* parse(QString *input);

        /**
        * Analiza la sección de la entrada referenciada por textRef con las
        * reglas sintácticas definidas en subRules. Retorna el árbol resultante
        * del reconocimento el texto refereciado o NULL si este no coincide con
        * la sintaxis definida. Si se identifica correctamente el texto la
        * referecia textRef es ubicada en el caracter siguiente al texto
        * reconocido.
        * @param textRef referecia al texto a analizar.
        * @param subRules reglas sintácticas para analizar el texto.
        * @return Devuelve el arbol de estructural del texto reconocido o NULL
        * si no se reconoce.
        */
        AstNode* process(QStringRef &textRef, QDomElement subRules);
};

#endif
