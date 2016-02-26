#include <iostream>
 
using namespace std;
 
enum Event {FLAG, NOFLAG, TIMEOUT, ESC, BYTECOMUM, BYTEESPECIAL, OVERFLOW, DESCARTABUFFER};
 
// A class abstrata que representa um estado
class State {
 public:
  virtual State * handle(Event & e) = 0;
};
 
// as classes concretas que implementam os estados da MEF
class Ocioso : public State {
 public:
  virtual State * handle(Event & e);
};
 
class Sucesso : public State {
 public:
  virtual State * handle(Event & e);
};
 
class Escape : public State {
 public:
  virtual State * handle(Event & e);
};
 
class Recepcao: public State {
 public:
  virtual State * handle(Event & e);
};
class Erro : public State {
 public:
  virtual State * handle(Event & e);
};
 

 
// A classe Context é a parte visível da MEF
class Context {
 protected:
  State * estado;
 public:
  Context(State * inicio) : estado(inicio) {}
 
  virtual ~Context() {}
 
  void Request(Event & e) {
    State * novo = estado->handle(e);
 
    if (novo) {
      delete estado;
      estado = novo;
    }
    else throw exception();
  }
};
 
State * Ocioso::handle(Event & e) {
  State * s;
  cout << "Ocioso -" << e << "-> ";
  switch (e) {
    case FLAG:
      s = new Sucesso;
      cout << "Sucesso" << endl;
      break;
    case NOFLAG:
      s = new Ocioso;
      cout << "Ocioso" << endl;
      break;
    default:
      s = new Ocioso;
      cout << "Ocioso" << endl;
      break;
  } 
  return s;
} 
 
State * Sucesso::handle(Event & e) {
  State * s;
  cout << "Sucesso -" << e << "-> ";
  switch (e) {
    case FLAG:
      s = new Sucesso;
      cout << "Sucesso" << endl;
      break;
    case TIMEOUT:
      s = new Ocioso;
      cout << "Ocioso" << endl;
      break;
    case BYTECOMUM:
      s = new Recepcao;
      cout << "Recepcao" << endl;
      break;  
    case ESC:
      s = new Escape;
      cout << "Escape" << endl;
      break;    
    default:
      s = new Sucesso;
      cout << "Sucesso" << endl;
      break;
  } 
  return s;
}
 
State * Recepcao::handle(Event & e) {
  State * s;
  cout << "Recepcao -" << e << "-> ";
  switch (e) {
    case FLAG:
      s = new Sucesso;
      cout << "Sucesso" << endl;
      break;
    case TIMEOUT:
      s = new Erro;
      cout << "Erro" << endl;
      break;
    case BYTECOMUM:
      s = new Recepcao;
      cout << "Recepcao" << endl;
      break;  
    case ESC:
      s = new Escape;
      cout << "Escape" << endl;
      break;
    case OVERFLOW:
      s = new Erro;
      cout << "Erro" << endl;
      break;
    default:
      s = new Recepcao;
      cout << "Recepcao" << endl;
      break;
  } 
  return s;
}
 
State * Escape::handle(Event & e) {
  State * s;
  cout << "Escape -" << e << "-> ";
  switch (e) {
    case BYTECOMUM:
      s = new Recepcao;
      cout << "Recepcao" << endl;
      break;
    case BYTEESPECIAL:
      s = new Erro;
      cout << "Erro" << endl;
      break;
    case TIMEOUT:
      s = new Erro;
      cout << "Erro" << endl;
      break;
    default:
      s = new Escape;
      cout << "Escape" << endl;
      break;
  } 
    return s;
}
  
 State * Erro::handle(Event & e) {
  State * s;
  cout << "Erro -" << e << "-> ";
  switch (e) {
    case DESCARTABUFFER:
      s = new Ocioso;
      cout << "Ocioso" << endl;
      break;
    default:
      s = new Erro;
      cout << "Erro" << endl;
      break;
  } 
    return s;
}


 
 int main() {
  State * s = new Ocioso;
  Context m(s);

  Event e = FLAG;
 
  //m.Request(e);
  //e = LOGINREQ;
  m.Request(e);
  e = BYTECOMUM;
  m.Request(e);
  e = BYTECOMUM;
  m.Request(e);
  //m.Request(e);
  //e = LOGINREQ;
  e = BYTECOMUM;
  m.Request(e);
  e = FLAG;
  m.Request(e);
  e = ESC;
  m.Request(e);
  e = BYTEESPECIAL;
  m.Request(e);
  e = DESCARTABUFFER;
  // m.Request(e); 
  // e = LOGOUTREQ;
  // m.Request(e);
  // e = LOGOUTRESP;
  // m.Request(e);   
  
  
  
 }


























// #include <iostream>
 
// using namespace std;
 
// enum Event { LOGINREQ, LOGINRESP, JOINREQ, JOINRESP, LISTREQ, LISTRESP, LEAVEREQ, LEAVERESP, LOGOUTREQ, LOGOUTRESP};
 
// // A class abstrata que representa um estado
// class State {
// public:
//   virtual State * handle(Event & e) = 0;
// };
 
// // as classes concretas que implementam os estados da MEF
// class State0 : public State {
// public:
//   // a instância de State0 somente pode ser obtida por meio de get_instance
//   static State0 * get_instance() {
//     if (not single) single = new State0();
//     return single;
//   }
 
//   virtual State * handle(Event & e);
// private:
//   // construtor, construtor de cópia e operador de atribuição ficam inacessíveis
//   // fora da classe
//   State0() {}
//   State0(const State0 & outro) {}
//   State0& operator=(const State0 & outro) {return *this;}
 
//   // ponteiro para a única instância da classe
//   public:
//   static State0 * single;
// };
 
// class State1 : public State {
//   public:
//   // a instância de State0 somente pode ser obtida por meio de get_instance
//   static State1 * get_instance() {
//     if (not single) single = new State1();
//     return single;
//   }
 
//   virtual State * handle(Event & e);
// private:
//   // construtor, construtor de cópia e operador de atribuição ficam inacessíveis
//   // fora da classe
//   State1() {}
//   State1(const State0 & outro) {}
//   State1& operator=(const State1 & outro) {return *this;}
 
//   // ponteiro para a única instância da classe
//   public:
//   static State1 * single;
 
// };
 
// class State2 : public State {
// public:
//   // a instância de State0 somente pode ser obtida por meio de get_instance
//   static State2 * get_instance() {
//     if (not single) single = new State2();
//     return single;
//   }
 
//   virtual State * handle(Event & e);
// private:
//   // construtor, construtor de cópia e operador de atribuição ficam inacessíveis
//   // fora da classe
//   State2() {}
//   State2(const State2 & outro) {}
//   State2& operator=(const State2 & outro) {return *this;}
 
//   // ponteiro para a única instância da classe
//   public:
//   static State2 * single;
// };
 
// class State3 : public State {
// public:
//   // a instância de State0 somente pode ser obtida por meio de get_instance
//   static State3 * get_instance() {
//     if (not single) single = new State3();
//     return single;
//   }
 
//   virtual State * handle(Event & e);
// private:
//   // construtor, construtor de cópia e operador de atribuição ficam inacessíveis
//   // fora da classe
//   State3() {}
//   State3(const State3 & outro) {}
//   State3& operator=(const State3 & outro) {return *this;}
 
//   // ponteiro para a única instância da classe
//   public:
//   static State3 * single;
// };
 
// State0* State0::single = NULL;
// State1* State1::single = NULL;
// State2* State2::single = NULL;
// State3* State3::single = NULL;
 
 
// // A classe Context é a parte visível da MEF
// class Context {
// protected:
//   State * estado;
// public:
//   Context(State * inicio) : estado(inicio) {}
 
//   virtual ~Context() {}
 
//   void Request(Event & e) {
//     State * novo = estado->handle(e);
 
//     if (novo) {
//       estado = novo;
//     }
//     else throw exception();
//   }
// };
 
// State * State0::handle(Event & e) {
//   cout << "0 -" << e << "-> ";
//   if (e == LOGINREQ) {
//     State1 * s = State1::get_instance();
//     cout << "1" << endl;
//     return s;
//   }
//   cout << "0" << endl;
//   return this;
// }
 
 
// State * State1::handle(Event & e) {
//   State * s;
//   cout << "1 -" << e << "-> ";
//   switch (e) {
//     case LOGINRESP:
//       s = State2::get_instance();
//       cout << "2" << endl;
//       break;
//     case ACK0:
//       s = State0::get_instance();
//       cout << "0" << endl;
//       break;
//     default:
//       s = State0::get_instance();
//       cout << "0" << endl;
//       break;
//   } 
//   return s;
// }
 
// State * State2::handle(Event & e) {
//   cout << "2 -" << e << "-> ";
//   if (e == MSG1) {
//     State3 * s = State3::get_instance();
//     cout << "3"<< endl;
//     return s;
//   }
//   cout << "2"<< endl;
//   return this;
// }
 
// State * State3::handle(Event & e) {
//   State * s;
//   cout << "3 -" << e << "-> ";
//   switch (e) {
//     case ACK1:
//       s = State2::get_instance();
//       cout << "2" << endl;
//       break;
//     case ACK0:
//       s = State0::get_instance();
//       cout << "0" << endl;
//       break;
//     default:
//       s = State2::get_instance();
//       cout << "0" << endl;
//       break;
//   } 
//   return s;
// }
 
// // int main() {
// //   State0 * s = State0::get_instance();
// //   Context m(s);
// //   Event e = LOGINREQ;
 
// //   m.Request(e);
// //   e = LOGINRESP;
// //   m.Request(e);
// //   e = ACK0;
// //   m.Request(e);
// //   e = MSG1;
// //   m.Request(e);
// //   e = ACK0;
// //   m.Request(e);
// // }