#!/bin/sh
# Script para aplicar un comando de sustitucion de cadenas dentro
# de ficheros recursivamente en subdirectorios

EXPRQUITAR="/home/jose/cybersim/head/"
EXPRPONER=""

EXPRQUITAR="${EXPRQUITAR//\//\\/}"
EXPRPONER="${EXPRPONER//\//\\/}"

# Crear un fichero temporal con las instrucciones de edicion para ed
instr=`tempfile`

cat << _FINBB > $instr
#!/bin/sh
cat << _FINED | ed -s \$1
,s/${EXPRQUITAR}/${EXPRPONER}/
wq
_FINED
_FINBB

chmod a+x $instr

# Busca recursivamente todos los ficheros regulares (no links, ni directorios)
find . -type f  -exec  $instr '{}' \;
# Borra el temporal de instrucciones
rm -f $instr

