/***************************************************************************
 *   Copyright (C) 2007 by Jose Manuel Cuadra Troncoso   *
 *   jose@portatil-jose   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef MATRIZ_H
#define MATRIZ_H

#include <iostream>
#include <vector>
#include <assert.h>

typedef unsigned int uint;

/**
  @author Jose Manuel Cuadra Troncoso <jose@portatil-jose>
*/
using namespace std;

template <class T>
class Matriz{
  protected:
    vector< vector<T> > matriz;
  public:
    Matriz() : n_filas(2), n_columnas(2) {
      matriz.resize(n_filas);
      for(uint i = 0; i < n_filas; i++)
        matriz[i].resize(n_columnas);
    };
    
    Matriz(uint n, uint m) : n_filas(n), n_columnas(m){
      matriz.resize(n_filas);
      for(uint i = 0; i < n_filas; i++)
        matriz[i].resize(n_columnas);
    };

    ~Matriz(){};
    
    void resize(uint n, uint m) {
      n_filas = n;
      n_columnas = m;
      matriz.resize(n_filas);
      for(uint i = 0; i < n_filas; i++) {
        matriz[i].resize(n_columnas);
      }
    };

    void fill2(const T& val) {
      for(uint i = 0; i < n_filas; i++) {
        for(uint j = 0; j < n_columnas; j++)
          matriz[i][j] = val;
      }    
    };    
    
    void fill(const T& val) {
      T *m;
      for(uint i = 0; i < n_filas; i++) {
        m = matriz[i].data();
        for(uint j = 0; j < n_columnas; j++)
          m[j] = val;
      }    
    }
    
    Matriz& operator=(const Matriz& A) {
      if(n_filas != A.getNFilas() || n_columnas != A.getNColumnas())
        resize(A.getNFilas(), A.getNColumnas());
        
      typename vector< vector<T> > ::const_iterator posrowA = A.getVector().begin();
      typename vector< vector<T> > ::iterator posrow = matriz.begin();
      typename vector<T>::const_iterator poscolA;
      typename vector<T>::iterator poscol;

      for(; posrowA < A.getVector().end(); ++posrowA, ++posrow) {
        const vector<T>& rowA = *(posrowA);
        vector<T>& row = *(posrow);
        poscolA = rowA.begin();
        poscol = row.begin();
        for(; poscolA < rowA.end(); ++poscolA, ++poscol)
          *poscol = *poscolA;
      }
      return *this;
    }


//     void operator=(Matriz* A) // & muy importante velocidad
//     {
//       T* rowData;
//       T* m;
//       assert(n_filas == A->getNFilas() && n_columnas == A->getNColumnas());
//       for(uint i = 0; i < n_filas; i++) {
//         rowData = (*A)[i].data();
//         m = matriz[i].data();
//         for(uint j = 0; j < n_columnas; j++)
//           m[j] = rowData[j];
//       }
// 
//     }    

    vector<T> & operator[](uint index) {
      assert(index < n_filas);
      return matriz[index];
    }

    void setRowData(uint index, T* rowData) {
      assert(index < n_filas);
      T* m = matriz[index].data();
      for(uint i = 0; i < n_columnas; i++)
        m[i] = rowData[i];
    }

/*    void setRowDataPtr(uint index, T* rowData) {
      assert(index < n_filas);
      matriz[index].data();

    } */   
    
    T* getRowData(uint index) {
      assert(index < n_filas);
      T* row = matriz[index].data();
      return row;
    }    

    T& operator()(uint i, uint j) {
      assert(i < n_filas);
      assert(j < n_columnas);
      return matriz[i][j];
    };

    Matriz operator+(Matriz B) {
      Matriz result;
      if(n_filas != B.getNFilas() || n_columnas != B.getNColumnas()) 
        cout << "No sumables" << endl;
      else {
        result.resize(n_filas, n_columnas);
        for(uint i = 0; i < n_filas; i++) {
          for(uint j = 0; j < n_columnas; j++) 
            result[i][j] += matriz[i][j] + B[i][j];
        } 
      }
      return result;
    };


    Matriz operator*(Matriz& B) {
      Matriz result;
      if(n_columnas != B.getNFilas()) 
        cout << "No multiplicables" << endl;
      else {
        result.resize(n_filas, B.getNColumnas());
        for(uint i = 0; i < n_filas; i++) {
          for(uint j = 0; j < B.getNColumnas(); j++) {
            for(uint k = 0; k < n_columnas; k++) {
              result[i][j] += matriz[i][k]*B[k][j];
            }
          }
        }
      } 
      return result;
    };
    
    Matriz operator*(T escalar) {
      Matriz result(n_filas, n_columnas);
      for(uint i = 0; i < n_filas; i++) {
        for(uint j = 0; j < n_columnas; j++)
            result[i][j] = matriz[i][j]*escalar;
      } 
      return result;
    };    

    Matriz transpose() {
      Matriz result(n_columnas, n_filas);
      for(uint i = 0; i < n_columnas; i++) {
        for(uint j = 0; j < n_filas; j++)
            result[i][j] = matriz[j][i];
      } 
      return result;
    };
    
    template <class Type>
    Matriz<Type> toType(Type) {
      Matriz<Type> result(n_filas, n_columnas);
        for(uint i = 0; i < n_filas; i++) {
          for(uint j = 0; j < n_columnas; j++) 
            result[i][j] = (Type) matriz[i][j];
        }
      return result;
    }
    
    friend ostream&
    operator<<(ostream& os, Matriz A) {
      for(uint i = 0; i < A.getNFilas(); i++) {
        os << "| ";
        for(uint j = 0; j < A.getNColumnas(); j++) {
          os << A[i][j] << "\t";
        }
        os << " |" << endl;
      }
      return os;
    };
    
    friend istream&
    operator>>(istream& is, Matriz A) {
      for(uint i = 0; i < A.getNFilas(); i++) {
        for(uint j = 0; j < A.getNColumnas(); j++) {
          is >> A[i][j];
        }
      }
  return is;     
    };
    
    uint getNFilas() const
    {
      return n_filas;
    }

    uint getNColumnas() const
    {
      return n_columnas;
    }
    
    const vector< vector < T > >& getVector() const
    {
      return matriz;
    }    
    
  protected:
    uint n_filas;
    uint n_columnas;
};


#endif
