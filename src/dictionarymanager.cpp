/**
 * @file
 * @author Isbel Ochoa Izquierdo
 */

#include <QDebug>

#include <dictionarymanager.h>

DictionaryManager::DictionaryManager(QDir dir) {

    this->directory = dir;
}

QRegExp DictionaryManager::getDictionary(QString key) {

    /* Se obtiene el diccionario cuyo nombre es el valor de 'key', en caso de
    ser válida la expresión regular obtenida esta es retornada.*/
    QRegExp regexp = dictionaries[key];
    if (!regexp.isEmpty()) {
        return regexp;
    }

    /* Si no se obtiene una expresión regular correcta se intenta cargala desde
    un fichero.*/
    return loadDictionary(key);
}

QRegExp DictionaryManager::loadDictionary(QString key) {

    /* Se intenta abrir el fichero correspondiente al diccionario cuyo nombre
    es 'key'.*/
    QFile dictionaryFile(directory.absoluteFilePath(key + ".dic"));
    if (!dictionaryFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "Parser: No se pudo abrir el fichero" << key + ".dic";
        return QRegExp();
    }

    /* Si se logra abrir el fichero se leen todos los elementos del diccionario
    y se genera una expresón regular.*/
    QString regexpStr;
    while (!dictionaryFile.atEnd()) {
        QByteArray line = dictionaryFile.readLine();
        if (line.isEmpty()) {
            continue;
        }

        /* Se escapan caracteres especiales.*/
        line.replace('\\', "\\\\");
        line.replace('(', "\\(");
        line.replace(')', "\\)");
        line.replace('{', "\\{");
        line.replace('}', "\\}");
        line.replace('[', "\\[");
        line.replace(']', "\\]");
        line.replace('|', "\\|");
        line.replace('.', "\\.");
        line.replace('*', "\\*");
        line.replace('+', "\\+");
        line.replace('?', "\\?");
        line.replace('^', "\\^");
        line.replace('$', "\\$");

        /* Se elimina el caracter de fin de linea.*/
        line.replace('\n', "");

        if (!regexpStr.isEmpty()) {
            line.push_front('|');
        }

        regexpStr.append(line);
    }
    dictionaryFile.close();
    regexpStr.push_front("\\b(");
    regexpStr.push_back(")\\b");

    QRegExp regexp = QRegExp(regexpStr);
    if (!regexp.isValid()) {
        qCritical() << "Parser: La expresion regular para el diccionario" <<
                       key << "no es correcta";
        return QRegExp();
    }

    /* Si la expresión regular generada es correcta se agrega en la tabla hash
    de diccionarios.*/
    dictionaries.insert(key, regexp);

    return regexp;
}
