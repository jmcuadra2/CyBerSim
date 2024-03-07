#ifndef MATRIZ3D_HPP
#define MATRIZ3D_HPP

#include <assert.h>
/**
	@author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/

using namespace std;

template <class T>
class Matriz3D{

  protected:
    vector< vector< vector<T> > > matriz;
  public:
    Matriz3D() : n_filas(0), n_columnas(0), n_capas(0) {
    };

    Matriz3D(uint n, uint m, uint k){
      resize(n, m, k);
    };    

    ~Matriz3D() {};

    void resize(uint n, uint m, uint k) {
      n_filas = n;
      n_columnas = m;
      n_capas = k;
      matriz.resize(n_filas);
      for(uint i = 0; i < n_filas; i++) {
        matriz[i].resize(n_columnas);
        for(uint j = 0; j < n_columnas; j++)
          matriz[i][j].resize(n_capas);
      }
    };    

    void fill(const T& val) {
      vector<T> *m;
      T *mm;
      for(uint i = 0; i < n_filas; i++) {
        m = matriz[i].data();
        for(uint j = 0; j < n_columnas; j++) {
          mm = m[j].data();
          for(uint k = 0; k < n_capas; k++)
            mm[k] = val;
        }
      }    
    };

    Matriz3D& operator = (const Matriz3D& A) {
      assert(n_filas == A.getNFilas() && n_columnas == A.getNColumnas() && n_capas == A.getNCapas());

      typename vector< vector< vector<T> > > ::const_iterator posrowA = A.getVector().begin();
      typename vector< vector< vector<T> > > ::iterator posrow = matriz.begin();
      typename vector< vector<T> >::const_iterator poscolA;
      typename vector< vector<T> >::iterator poscol;
      typename vector<T>::const_iterator poscapaA;
      typename vector<T>::iterator poscapa;
            
      for(; posrowA < A.getVector().end(); ++posrowA, ++posrow) {
        const vector< vector<T> >& rowA = *(posrowA);
        vector< vector<T> >& row = *(posrow);
        poscolA = rowA.begin();
        poscol = row.begin();
        for(; poscolA < rowA.end(); ++poscolA, ++poscol) {
          const vector<T>& colA = *(poscolA);
          vector<T>& col = *(poscol);
          poscapaA = colA.begin();
          poscapa = col.begin();
          for(; poscapaA < colA.end(); ++poscapaA, ++poscapa)
            *poscapa = *poscapaA;
        }
      }
      return *this;            

    };

    vector < vector<T> > & operator[](uint index) {
      assert(index < n_filas);
      return matriz[index];
    }
      
//     vector<T> & operator[](uint i, uint j) {
//       assert(index < n_columnas);
//       assert(j < n_columnas);
//       return matriz[i][j];
//     }
       
    T& operator()(uint i, uint j, uint k) {
      if(j >= n_columnas)
        int kk = 0;
        
      assert(i < n_filas);
      assert(j < n_columnas);
      assert(k < n_capas);
      return matriz[i][j][k];
    };       
  
/*    void setRowData(uint index, T* rowData) {
      assert(index < n_filas);
      T* m = matriz[index].data();
      for(uint i = 0; i < n_columnas; i++)
        m[i] = rowData[i];
    };
    
    T* getRowData(uint index) {
      assert(index < n_filas);
      T* row = matriz.at(index).data();
      return row;
    }; */
     
    uint getNFilas() const
    {
      return n_filas;
    }

    uint getNColumnas() const
    {
      return n_columnas;
    }     

    uint getNCapas() const
    {
      return n_capas;
    }
  protected:
    uint n_filas;
    uint n_columnas;
    uint n_capas;
    
};

#endif
