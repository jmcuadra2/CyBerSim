/***************************************************************************
                          neuraldisdoc.h  -  description
                             -------------------
    begin                : mar oct 15 04:16:08 CEST 2002
    copyright            : (C) 2002 by Jose M. Cuadra Troncoso
    email                : jmcuadra@dia.uned.es
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef NEURALDISDOC_H
#define NEURALDISDOC_H

// include files for QT
#include <QObject>
#include <QDomDocument>


// application specific includes

class Settings;

/**
  * the Document Class
  */

class NeuralDisDoc : public QObject
{
    Q_OBJECT

  protected:  
    NeuralDisDoc();
    
  public:
    static NeuralDisDoc* instance(void);
    ~NeuralDisDoc();
    void newDoc(void);
//    bool save(QString robotDir);
    bool save(void);
    bool saveAs(const QString &filename);    
//    bool saveAs(const QString &filename, QString robotDir);
    bool load(QString &filename);
    bool loadSim(QString &filename);
    bool isModified(void) const;
    void setModified(bool mod);
    QString getFileName(void);
    bool getFileDialog(QString& file_name, const QString& sender, QString type_file = "", QString extension = "");
    QDomDocument& getXMLNet(void) { return netDoc ; };
    QDomDocument& getXMLSim(void) { return simDoc ; };
    QDomDocument loadXML(QString& file_name, const QString& doc_name, const QString& sender, QString type_file = "", QString extension = "");
    const QString saveDocument(QString& file_name, QString content, const QString& doc_type, const QString& extension, const QString& sender, bool change_name = false);
    bool testXML(void);   
    bool needChangeName(const QString& file_name);

  protected:
    bool load_xml(QString& file_name,
                    QDomDocument& doc, const QString& doc_name, const QString& sender);
    bool noFileDialog(const QString& sender);

  public slots:
    void textoChange(QString texto);
    void initView(void);
    void netClosed(void);
    bool saveNet(void);

  signals:
    void documentChanged(const QString& texto);
    void documentOpen(const QString& texto);
    void documentOpenOld(QString texto);
    void documentName(QString f_name);

  protected:
    static NeuralDisDoc* docs_manager;

  private:
    bool modified;
    QString fileName;
    QString texto;
    QDomDocument netDoc;
    QDomDocument simDoc;
    Settings* settings;
     

};

#endif
