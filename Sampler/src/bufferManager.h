#ifndef BUFFER_MANGER_H_
#define BUFFER_MANGER_H_
#include <vector>
#include <stdint.h>
#include <string.h>


//#define DEBUG_BUFFER_MANAGER

class bufferManager
{
  public:
    bufferManager(unsigned int * p, unsigned max){
      m_p = p;
      m_pNext = m_p;
      m_size = max;
      
      *m_p = 0;

#ifdef DEBUG_BUFFER_MANAGER
      sprintf(str_, "Init bufferManager: %p size(%u)\n", m_p, m_size);   
      Serial.print(str_);   
#endif
    }

    ~bufferManager(){}

    unsigned int * getNextPointer(void){return m_pNext;}
    unsigned int * getMemPointer(void){return m_p;}

    unsigned int * getPointerFromIndex(int i){
      if(i < 0 || i >= (int)m_pList.size()){
        return NULL;        
      }

#ifdef DEBUG_BUFFER_MANAGER
        sprintf(str_, "bufferManager getPointer: %p %d %d\n", m_pList.at(i), i, m_pList.size());   
        Serial.print(str_);   
#endif          
      return m_pList.at(i);
    }

    int getIndexFromPointer(unsigned int * p){
      int i = 0;
      for( auto pr : m_pList )
      {
        if(p == pr ){return i;}
        i++;
      }
      return (-1);
    }

    bool allocate(int size){
      if(    (size > 0) 
         || ((size + m_allocated) < m_size) )
      {
        m_pList.push_back(m_pNext);

#ifdef DEBUG_BUFFER_MANAGER
        sprintf(str_, "bufferManager allocate: %p size(%u) items(%d)\n", m_pNext, size, m_pList.size());   
        Serial.print(str_);   
#endif        

        m_pNext += size;
        m_allocated += size;
        return true;
      }
      return false;
    }

    void free(void){
      m_pNext = m_p;
      *m_p = 0;

      m_allocated = 0;
    }

    unsigned int fillLevel(void){return m_allocated;}


  private:
    unsigned int * m_p {NULL};
    unsigned int * m_pNext {NULL};
    std::vector<unsigned int *> m_pList;

    unsigned int m_size {0};
    unsigned int m_allocated {0};

#ifdef DEBUG_BUFFER_MANAGER
    char str_[100];
#endif

};


#endif /*BUFFER_MANAGER_H_*/
