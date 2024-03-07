/***************************************************************************
                          subnet.cpp  -  description
                             -------------------
    begin                : July 8 2005
    copyright            : (C) 2005 by Jose M. Cuadra Troncoso
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

#include "subnet.h"
#include "netfactory.h"
#include "../disview/gui/neuronvaluesdialog.h"
#include "../neuraldis/xmloperator.h"

#include <QMessageBox>

SubNet::SubNet(unsigned long n_id, int lay,const QString& path, const QString& filename)
{

    infoWhatsThis = "Hola subred "+QString::number(n_id);
    file_name = filename ;
    path_file = path ;
    selected = false;

    lay = 0;
    id = n_id;
    initValues();
}


SubNet::SubNet(ulong n_id)
{
    file_name = "" ;
    path_file = "" ;

    id = n_id;
    initValues();
} 


void SubNet::initValues(void)
{
    type = 1;

    qc_pix = 0;
    qc_pix_sel = 0;
    qc_pix_a.clear();
    qc_subnet_pix = 0;
    qc_texto =0;

    num_InputN = 0;
    num_HiddenN = 0;
    num_OutputN = 0;

    if ( path_file != "" && file_name != "" ){
        readSubNet();
    }

}


SubNet::~SubNet()
{

    if (qc_pix)
        delete (qc_pix);
    qc_pix = 0;
    if (qc_pix_sel)
        delete (qc_pix_sel);
    qc_pix_sel = 0;
    if (qc_subnet_pix)
        delete (qc_subnet_pix);
    qc_subnet_pix = 0;
    if (qc_texto)
        delete (qc_texto);
    qc_texto= 0;
    if (!qc_pix_a.empty()) {
        QListIterator<QPixmap *> ita(qc_pix_a);
        while(ita.hasNext()){
            delete ita.next();
        }
    }
    qc_pix_a.clear();

}

void SubNet::write(QDomDocument& doc, QDomElement& e)
{

    XMLOperator* xml_operator = XMLOperator::instance();

    QDomElement tag_subnet = doc.createElement("SubNet");
    e.appendChild(tag_subnet);
    tag_subnet.appendChild(xml_operator->createTextElement(doc,
                                                           "type", QString::number(rtti_dev())));
    tag_subnet.appendChild(xml_operator->createTextElement(doc,
                                                           "num_id", QString::number(id)));
    tag_subnet.appendChild(xml_operator->createTextElement(doc,
                                                           "path",path_file));
    tag_subnet.appendChild(xml_operator->createTextElement(doc,
                                                           "filename",file_name));
    // neuronas de cada capa de la subred pendiente
    tag_subnet.appendChild(xml_operator->createTextElement(doc,
                                                           "number_input", QString::number(num_InputN)));
    tag_subnet.appendChild(xml_operator->createTextElement(doc,
                                                           "number_hidden", QString::number(num_HiddenN)));
    tag_subnet.appendChild(xml_operator->createTextElement(doc,
                                                           "number_output", QString::number(num_OutputN)));

    // si existen sinapsis externas las tengo que añadir en Sub como en las neuronas
    QDomElement tag_subnet_sub = doc.createElement("SubObjects");
    tag_subnet.appendChild(tag_subnet_sub);
    write_subobjects(doc, tag_subnet_sub);

}

// nuevo para crear subobjetos de la subred 

void SubNet::write_subobjects(QDomDocument& doc, QDomElement& e)
{

    QDomElement tag_ax = doc.createElement("Axon");
    e.appendChild(tag_ax);

    QListIterator<SubNetSynapse *> its(ExternalSynapses);
    SubNetSynapse* syn;
    while(its.hasNext()) {
        syn = its.next();
        syn->write(doc, tag_ax);
    }

}  

bool SubNet::getSelected(void)
{
    return selected;
}

void SubNet::setSelected(bool sel)
{
    selected = sel;
}

bool SubNet::setParams(const QDomElement& e)
{
    /*
   Ejemplo de estructura para subnet
   <SubNet>
      <type>4</type>
      <num_id>1</num_id>
      <path>/home/lola/proyecto/csim/examples/nets/</path>
      <filename>buscaluz.net</filename>
   </SubNet>
   */

    // e es el nodo SubNet
    XMLOperator* xml_operator = XMLOperator::instance();

    QDomElement e_type = xml_operator->findTag("type", e);
    QDomElement e_num_id = xml_operator->findTag("num_id", e);
    QDomElement e_path = xml_operator->findTag("path", e);
    QDomElement e_filename = xml_operator->findTag("filename", e);

    id = e_num_id.text().toUInt();
    type = e_type.text().toInt();
    path_file = e_path.text();
    file_name = e_filename.text();

    // faltarian algunas comprobaciones minimas
    return true;
}


int SubNet::rtti_dev(void) const
{

    return (type);

}


bool SubNet::addSynapse (SubNetSynapse *synapse)
{
    if (synapse) {
        ExternalSynapses.append(synapse);
        synapse->setId(ExternalSynapses.count());
        return true;
    }

    return false;
}


bool SubNet::delSynapse (SubNetSynapse */*synapse*/)
{
    // todo
    return true;
}

// hay que crear el cuadro de diálogo para añadir una nueva SubNet este no vale
QewExtensibleDialog* SubNet::createDialog(bool mode, bool added)
{

    QewDialogFactory *factory = new QewDialogFactory();
    QewExtensibleDialog* main_dialog = factory->createDialog(QewExtensibleDialog::QewSimpleWidget);
    if(main_dialog->setUpDialog()) {
        neuronValuesDialog *dialog = new neuronValuesDialog(main_dialog);
        dialog->setUpDialog();
        dialog->init((Neuron*) this, mode, added);
        main_dialog->addExtensibleChild(dialog, main_dialog->windowTitle());
    }
    else {
        delete main_dialog;
        main_dialog = 0;
    }
    return main_dialog;
}

void SubNet::setPath(const QString path) 
{ 
    path_file = path ;
}

const QString& SubNet::getPath(void) 
{ 
    return path_file;
}

void SubNet::setFileName(const QString filename) 
{ 
    file_name = filename;
}

const QString& SubNet::getFileName(void) 
{ 
    return file_name;
}


void SubNet::setFile (QString path, QString file)
{
    path_file = path;
    file_name = file;
}

void SubNet::num_idChange(ulong prev_n_i)
{
    id = prev_n_i + 1;
    // he descomentado esto para que cambie el num_id si le doy el mismo original idChanged
    emit num_idChanged(id);
}


void SubNet::setPix (QString pix_file, QString pix_sel_file)
{
    listpixmaps.clear();
    if (qc_pix) { delete (qc_pix); }
    qc_pix = 0;
    if (qc_pix_sel) { delete (qc_pix_sel); }
    qc_pix_sel = 0;
    if (!qc_pix_a.empty()) {
        QListIterator<QPixmap *> ita(qc_pix_a);
        while(ita.hasNext()){
            delete ita.next();
        }
    }
    qc_pix_a.clear();

    if (qc_subnet_pix) { delete (qc_subnet_pix); }
    qc_subnet_pix = 0;
    // añado el título
    if (qc_texto){  delete (qc_texto);}
    qc_texto= 0;

    qc_pix = new QPixmap (pix_file);
    listpixmaps.append(*qc_pix);

    qc_pix_sel = new QPixmap (pix_sel_file);
    listpixmaps.append(*qc_pix_sel);
    QPixmap px;
    int i = 0;
    while(i < listpixmaps.size()){
        px = listpixmaps.takeFirst();
        qc_pix_a.append(&px);
        i++;
    }

    width = qc_pix->width();
    height = qc_pix->height();

}


void SubNet::draw(QGraphicsView* view)
{
    QFont font("Arial", 10);

    bool can_draw_pixmaps = true;
    if (qc_pix_a.empty())
    {
        can_draw_pixmaps = false;
    }

    // elegir el pixmap en funcion de si esta seleccionada
    if (can_draw_pixmaps) {
        if(!qc_subnet_pix) {
            qc_subnet_pix = new AnimatedPixmapItem (qc_pix_a);
            view->scene()->addItem(qc_subnet_pix);
            qc_subnet_pix->setZValue(10);
        }
        qc_subnet_pix->setPos(qreal(x),qreal(y));

        if (getSelected()) {
            qc_subnet_pix->setFrame(1);
        }
        else {
            qc_subnet_pix->setFrame(0);
        }

        qc_subnet_pix->show();

    }

    if (!qc_texto) {
        qc_texto = new QGraphicsSimpleTextItem(getFileName().left(getFileName().length()-4));
        view->scene()->addItem(qc_texto);
        qc_texto->setPen(QPen(QColor("black")));

        font.setPixelSize(8);
        qc_texto->setFont (font);

    }
    qc_texto->setPos(x + getWidth() /2 - 14,y + getHeight());
    qc_texto->setZValue(30);

    qc_texto->show();

    /* pendiente titulo subred
   // mostrar texto
   if (!qc_texto)
   {
      //qc_texto = new QCanvasText(QString::number(num_id), view->canvas());
      //qc_texto->setColor(*color);
      qc_texto->setColor(QColor("black"));
      qc_texto->setFont (font);

   }
   //qc_texto->setText(QString::number(num_id));
   
   if (num_id < 10)
   {
         qc_texto->setX(x + width()/2 - 3);
         qc_texto->setY(y + height()/2 - 7 );
   }
   else
   {
         qc_texto->setX(x + width()/2 - 8);
         qc_texto->setY(y + height()/2 - 7);
   }

   qc_texto->setZ(30);
   qc_texto->show();

*/

}

void SubNet::drawSynapses (QGraphicsView* view)
{
    QListIterator<SubNetSynapse *> its (ExternalSynapses);
    SubNetSynapse* synapse;
    while(its.hasNext()) {
        synapse = its.next();
        synapse->draw(view);
    }
}

void SubNet::readSubNet(void)
{
    bool ok;

    XMLOperator *xml_operator = XMLOperator::instance();

    QDomDocument doc( "subnet" );
    QFile file( path_file+file_name );
    if ( !file.open( QIODevice::ReadOnly ) )
        return;
    if ( !doc.setContent( &file ) ) {
        file.close();
        return;
    }
    file.close();

    QDomElement root = doc.documentElement();
    QDomElement netInfo = root;

    QDomElement e_ninput = xml_operator->findTag("number_input", netInfo);
    QDomElement e_nhidden = xml_operator->findTag("number_hidden", netInfo);
    QDomElement e_noutput = xml_operator->findTag("number_output", netInfo);
    // guardo los valores dentro de los atributos de la clase
    num_InputN = e_ninput.text().toUInt(&ok);
    num_HiddenN = e_nhidden.text().toUInt(&ok);
    num_OutputN = e_noutput.text().toUInt(&ok);

}

unsigned long SubNet::getNumInputNeurons(void)
{
    return num_InputN;
}

unsigned long SubNet::getNumHiddenNeurons(void)
{
    return num_HiddenN;
}

unsigned long SubNet::getNumOutputNeurons(void)
{
    return num_OutputN;
}

// nuevo método para construir los axones de las subredes
void SubNet::initSubObjects(const QDomElement& e)
{

    NetFactory* factory = NetFactory::instance();
    XMLOperator* xml_operator = XMLOperator::instance();
    QDomElement e_ax = xml_operator->findTag("Axon", e);
    factory->buildAxon(ExternalSynapses, e_ax);

    QListIterator<SubNetSynapse *> it(ExternalSynapses);
    SubNetSynapse* syn;
    while(it.hasNext()) {
        syn = it.next();
        syn->setPointerFromSubNet(this);
    }
}
