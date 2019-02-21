/**
 * @file
 * @author Isbel Ochoa Izquierdo
 */

#include <QDebug>
#include <QDir>
#include <QTime>

#include <parsermanager.h>
#include <parser.h>
#include <astnode.h>
#include <dictionarymanager.h>

ParserManager::ParserManager(QDir confDir)
{
    configDirectory = confDir;
    dictionaries = new DictionaryManager(confDir);

    /* Se buscan todos los archivos *.xml en el directorio de configuración.*/
    confDir.setNameFilters(QStringList("*.xml"));
    QFileInfoList cfgList = confDir.entryInfoList();

    /* Se comprueba la existencia de algún fichero de configuración*/
    if (cfgList.isEmpty()) {
        qCritical() << QObject::trUtf8(
                           "Parser: No se han encontrado ficheros configuración en %1 ").arg(
                           confDir.absolutePath());
    }

    /* De cada fichero xml se extraen las reglas sintácticas del formato que
    describe.*/
    for (int i = 0; i < cfgList.size(); ++i) {
        QString format = cfgList.at(i).baseName();
        configureParser(format);
    }
}

ParserManager::~ParserManager()
{
    for (int i = 0; i < parserList.size(); ++i) {
        delete parserList.at(i);
    }
    delete dictionaries;
}

QDomElement ParserManager::loadConfiguration(QString format)
{
    QDomDocument formatDoc("configdoc");
    QFile formatFile(configDirectory.absoluteFilePath(format + ".xml"));

    if (!formatFile.open(QIODevice::ReadOnly)) {
        qCritical() << QObject::trUtf8(
                           "Parser: No se puedo abrir el fichero de configuración %1.xml.").arg(
                           format);

    } else if (!formatDoc.setContent(&formatFile)) {
        qCritical() << QObject::trUtf8(
                           "Parser: La sintaxis del fichero %1.xml no es correcta.").arg(format);
        formatFile.close();
    }

    return formatDoc.documentElement();
}

int ParserManager::findParser(QString format)
{
    int pos = 0;
    while (pos < parserList.size() && parserList.at(pos)->getFormat() !=
           format) {
        pos++;
    }

    return pos < parserList.size() ? pos : -1;
}

bool ParserManager::configureParser(QString format)
{
    int pos = findParser(format);

    QDomElement confrules = loadConfiguration(format);

    /* Se comprueba si las reglas están correctamente formadas.*/
    if (confrules.isNull()) {
        return false;
    }

    if (pos == -1) {
        /* Se genera un analizador sintáctico para el fomato descrito.*/
        Parser * formParser = new Parser(confrules, dictionaries);
        parserList.append(formParser);
        return true;
    }

    parserList.at(pos)->setRules(confrules);
    return true;
}

int ParserManager::parseFormat(QString *input, QDomDocument &doc, int parserPos)
{
    int formatCount = 0;
    Parser * formatParser = parserList.at(parserPos);
    QRegExp formatExp = formatParser->matchExp();

    if (!formatExp.isEmpty() && formatExp.isValid()) {
        int pos = 0;
        int n = 0;

        /* Se separa cada ocurrencia del formato dentro de la entrada de
        texto.*/
        while ((pos = input->indexOf(formatExp, pos)) != -1 &&
               (n = formatExp.matchedLength()) > 0) {

            QTime start = QTime::currentTime();

            QString formatOcur(input->mid(pos, n));
            AstNode * tree = formatParser->parse(&formatOcur);

            /* Si se obtiene un árbol de sintaxis correctamente formado, su
            estructura es agregada al resultado final.*/
            if (tree) {
                QDomElement ocurElem = doc.createElement(
                            formatParser->getFormat());
                doc.documentElement().appendChild(ocurElem);
                QDomElement frmtInput = doc.createElement("inputdata");
                frmtInput.appendChild(doc.createTextNode(formatOcur));
                ocurElem.appendChild(frmtInput);
                QDomElement frmtOutput = tree->toDom(&doc);
                frmtOutput.setAttribute("milisecs", start.msecsTo(QTime::currentTime()));
                ocurElem.appendChild(frmtOutput);
                delete tree;
                formatCount++;
            }
            pos += n;
        }
    }
    return formatCount;
}

int ParserManager::parseFormat(QString *input, QDomDocument &doc, QString format)
{
    int pos = findParser(format);

    /* Se verifica que se haya encontrado el paraser para el formato. */
    if (pos == -1) {
        qCritical() << QObject::trUtf8(
                           "Parser: No se puedo encontrar un parser para el formato %1.").arg(
                           format);
        return -1;
    }

    return parseFormat(input, doc, pos);
}

QDomDocument ParserManager::toDom(QString *input)
{
    /* Se crean el documento DOM y su elemento raíz donde se almacenará el
    resultado final.*/
    QDomDocument doc;
    QDomElement root = doc.createElement("xml");
    doc.appendChild(root);

    /* Se procesa la entrada de texto con cada uno de los analizadores
    sintácicos disponibles.*/
    for (int i = 0; i < parserList.size(); ++i) {
        parseFormat(input, doc, i);
    }

    return doc;
}

QDomDocument ParserManager::parseAll(QString *input)
{
    /* Se crean el documento DOM y su elemento raíz donde se almacenará el
    resultado final.*/
    QDomDocument doc;
    QDomElement root = doc.createElement("xml");
    doc.appendChild(root);

    int startpos = 0;

    do {
        int formatpos = -1;
        int minpos = input->length();
        int majlength = 0;

        for (int i = 0; i < parserList.size(); ++i) {
            QRegExp formatExp = parserList.at(i)->matchExp();
            if (formatExp.isEmpty() || !formatExp.isValid()) {
                continue;
            }
            int pos = input->indexOf(formatExp, startpos);
            int length = formatExp.matchedLength();
            if (pos >= 0 && ((pos < minpos) || (pos == minpos && length > majlength))) {
                formatpos = i;
                minpos = pos;
                majlength = length;
            }
        }

        if (formatpos == -1 ) {
            QDomElement unknow = doc.createElement("unknow");
            unknow.appendChild(doc.createTextNode(input->mid(startpos)));
            doc.documentElement().appendChild(unknow);
            break;
        }

        if (minpos > startpos) {
            QDomElement unknow = doc.createElement("unknow");
            unknow.appendChild(doc.createTextNode(input->mid(startpos, minpos - startpos)));
            doc.documentElement().appendChild(unknow);
        }

        QTime start = QTime::currentTime();

        QString formatOcur(input->mid(minpos, majlength));
        AstNode * tree = parserList[formatpos]->parse(&formatOcur);

        /* Si se obtiene un árbol de sintaxis correctamente formado, su
        estructura es agregada al resultado final.*/
        if (tree) {
            QDomElement ocurElem = doc.createElement(parserList[formatpos]->getFormat());
            doc.documentElement().appendChild(ocurElem);
            QDomElement frmtInput = doc.createElement("inputdata");
            frmtInput.appendChild(doc.createTextNode(formatOcur));
            ocurElem.appendChild(frmtInput);
            QDomElement frmtOutput = tree->toDom(&doc);
            frmtOutput.setAttribute("milisecs", start.msecsTo(QTime::currentTime()));
            ocurElem.appendChild(frmtOutput);
            delete tree;
        }

        startpos = minpos + majlength;
    } while (true);

    return doc;
}

QByteArray ParserManager::toXml(QString *input) {
    return toDom(input).toByteArray();
}

bool ParserManager::loadDictionary(QString key) {
    return !(dictionaries->loadDictionary(key).isEmpty());
}


