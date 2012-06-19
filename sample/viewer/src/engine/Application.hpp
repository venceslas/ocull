#ifndef ENGINE_APPLICATION_HPP_
#define ENGINE_APPLICATION_HPP_

namespace engine {

class Application
{  
  public:
    Application() {}
    virtual ~Application() {}
    
    // ++ Intializer ++    
    void init(int argc, char *argv[]);
    
    // ++ MainLoop ++
    virtual void run() = 0;    

  protected:
    // ++ Sub initializers ++
    virtual void _initContext( int argc, char *argv[]);
    virtual void _initOpenGL( int argc, char *argv[]);
    virtual void _initCUDA( int argc, char *argv[]);
    virtual void _initData( int argc, char *argv[]);
        
  private:
    // ++ Disallow copy & affectation ++
    Application(const Application&);
    Application& operator= (const Application&);
};

}

#endif //ENGINE_APPLICATION_HPP_
