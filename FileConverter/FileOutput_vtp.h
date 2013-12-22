#ifndef FILE_OUTPUT_VTP_ASCII_H
#define FILE_OUTPUT_VTP_ASCII_H
#include <string>
#include <list>
#include <iostream>
#include <fstream>
#include <typeinfo>

namespace PPlib
{
  class ParticleData;
}
namespace LPT
{
  template <typename T>
  static 
  std::string get_type(T data)
  {
    std::string type;
    if(typeid(data)==typeid(int))
    {
      type = "Int32";
    } else if(typeid(data)==typeid(long)){
      type = "Int64";
    } else if(typeid(data)==typeid(unsigned int)){
      type = "UInt32";
    } else if(typeid(data)==typeid(unsigned long)){
      type = "Unt64";
    } else if(typeid(data)==typeid(float)){
      type = "Float32";
    } else if(typeid(data)==typeid(double)){
      type = "Float64";
    }
    return type;
  }

  //! ファイル出力用の基底クラス
  class BaseFileOutput
  {
    public:
      BaseFileOutput() {}
      virtual ~BaseFileOutput() {}

      virtual void write(std::list<PPlib::ParticleData *> *Particles, std::string filename) 
      {
        //既存のファイルがあったら削除する
        std::ofstream ofs(filename.c_str(),std::ios::trunc);
        ofs.close();
        std::cerr <<"writing: "<<filename<<std::endl;
      }

    private:
      /// noncopyable 
      BaseFileOutput(const BaseFileOutput &obj);
      BaseFileOutput & operator=(const BaseFileOutput &obj);

  };

  class VTK_PolyDataFileHeader : public BaseFileOutput
  {
    public:
      explicit VTK_PolyDataFileHeader(BaseFileOutput *obj) : base(obj) {}
      ~VTK_PolyDataFileHeader()
      {
        delete base;
      }

      void write(std::list<PPlib::ParticleData *> *Particles, std::string filename)
      {
        base->write(Particles, filename);
        std::ofstream ofs(filename.c_str(),std::ios::app);
        ofs <<"<?xml version=\"1.0\"?>"<<std::endl;
        ofs <<"<VTKFile type=\"PolyData\" version=\"0.1\">"<<std::endl;
        ofs <<"<PolyData>"<<std::endl;
        ofs <<"<Piece NumberOfPoints=\""<<Particles->size()<<"\" NumberOfVerts=\"0\" NumberOfLines=\"0\" NumberOfStrips=\"0\" NumberOfPolys=\"0\">"<<std::endl;
      }
    private:
      VTK_PolyDataFileHeader();
      BaseFileOutput * base;
  };

  class VTK_PolyDataFileFooter : public BaseFileOutput
  {
    public:
      explicit VTK_PolyDataFileFooter(BaseFileOutput *obj) : base(obj) {}

      ~VTK_PolyDataFileFooter()
      {
        delete base;
      }

      void write(std::list<PPlib::ParticleData *> *Particles, std::string filename)
      {
        base->write(Particles, filename);
        std::ofstream ofs(filename.c_str(),std::ios::app);
        ofs <<"</Piece>"<<std::endl;
        ofs <<"</PolyData>"<<std::endl;
        ofs <<"</VTKFile>"<<std::endl;
      }
    private:
      VTK_PolyDataFileFooter();
      BaseFileOutput * base;
  };

  class VTK_Points : public BaseFileOutput
  {
    public:
      explicit VTK_Points(BaseFileOutput *obj) : base(obj) {}

      ~VTK_Points()
      {
        delete base;
      }
      void write(std::list<PPlib::ParticleData *> *Particles, std::string filename)
      {
        base->write(Particles, filename);
        std::ofstream ofs(filename.c_str(),std::ios::app);
        ofs <<"<Points>"<<std::endl;
        ofs <<"<DataArray ";
        ofs <<"type=\""<<get_type((*(Particles->begin()))->x)<<"\" ";
        ofs <<"NumberOfComponents=\"3\" ";
        ofs <<"format=\"ascii\">"<<std::endl;
        for (std::list<PPlib::ParticleData *>::iterator it=Particles->begin();it!=Particles->end();++it)
        {
          ofs << (*it)->x << " ";
          ofs << (*it)->y << " ";
          ofs << (*it)->z << " ";
        }
        ofs <<std::endl;
        ofs <<"</DataArray>"<<std::endl;
        ofs <<"</Points>"<<std::endl;
      }

    private:
      VTK_Points();
      BaseFileOutput * base;
  };

  class VTK_PointDataHeader : public BaseFileOutput
  {
    public:
      explicit VTK_PointDataHeader(BaseFileOutput *obj) : base(obj) {}

      ~VTK_PointDataHeader()
      {
        delete base;
      }
      void write(std::list<PPlib::ParticleData *> *Particles, std::string filename)
      {
        base->write(Particles, filename);
        std::ofstream ofs(filename.c_str(),std::ios::app);
        ofs <<"<PointData ";
        //TODO 出力する物理量を変える時のために、コンストラクタで物理量の一覧をもらってくるように変える
        ofs <<"Vectors=\"ParticleVelocity\"> ";
        ofs <<">"<<std::endl;
      }

    private:
      VTK_PointDataHeader();
      BaseFileOutput * base;
  };

  class VTK_PointDataFooter : public BaseFileOutput
  {
    public:
      explicit VTK_PointDataFooter(BaseFileOutput *obj):base(obj) {}

      ~VTK_PointDataFooter()
      {
        delete base;
      }
      void write(std::list<PPlib::ParticleData *> *Particles, std::string filename)
      {
        base->write(Particles, filename);
        std::ofstream ofs(filename.c_str(),std::ios::app);
        ofs <<"</PointData>"<<std::endl;
      }

    private:
      VTK_PointDataFooter();
      BaseFileOutput * base;
  };

  class VTK_DataArrayParticleVelocity: public BaseFileOutput
  {
    public:
      explicit VTK_DataArrayParticleVelocity(BaseFileOutput *obj) : base(obj) {}
      ~VTK_DataArrayParticleVelocity()
      {
        delete base;
      }
      void write(std::list<PPlib::ParticleData *> *Particles, std::string filename)
      {
        base->write(Particles, filename);
        std::ofstream ofs(filename.c_str(),std::ios::app);
        ofs <<"<DataArray ";
        ofs <<"type=\"Float32\" ";
        ofs <<"Name=\"Particle Velocity\" ";
        ofs <<"NumberOfComponents=\"3\" ";
        ofs <<"format=\"ascii\">"<<std::endl;
        for (std::list<PPlib::ParticleData *>::iterator it=Particles->begin();it!=Particles->end();++it)
        {
          ofs << (*it)->Vx << " "<< (*it)->Vy << " "<< (*it)->Vz << " ";
        }
        ofs <<std::endl;
        ofs <<"</DataArray>"<<std::endl;
      }

    private:
      VTK_DataArrayParticleVelocity();
      BaseFileOutput * base;
  };

  template <typename T>
  class VTK_DataArrayScalarByASCII: public BaseFileOutput
  {
    public:
      explicit VTK_DataArrayScalarByASCII(const char * name, T PPlib::ParticleData::* member_pointer, BaseFileOutput *obj)
        : name(name), member_pointer(member_pointer), base(obj) {}

      ~VTK_DataArrayScalarByASCII()
      {
        delete base;
      }
      void write(std::list<PPlib::ParticleData *> *Particles, std::string filename)
      {
        base->write(Particles, filename);
        std::ofstream ofs(filename.c_str(),std::ios::app);
        ofs <<"<DataArray ";
        ofs <<"type=\""<<get_type((*Particles->begin())->*member_pointer)<<"\" ";
        ofs <<"Name=\""<<name<<"\" ";
        ofs <<"format=\"ascii\">"<<std::endl;
        for (std::list<PPlib::ParticleData *>::iterator it=Particles->begin();it!=Particles->end();++it)
        {
          ofs << (*it)->*member_pointer << " ";
        }
        ofs <<std::endl;
        ofs <<"</DataArray>"<<std::endl;
      }

    private:
      VTK_DataArrayScalarByASCII();
      BaseFileOutput * base;
      std::string name;
      T PPlib::ParticleData::* member_pointer;
  };

  template <typename T>
  class VTK_DataArrayVectorByASCII: public BaseFileOutput
  {
    public:
      explicit VTK_DataArrayVectorByASCII(const char * name, T PPlib::ParticleData::* member_pointer0, T PPlib::ParticleData::* member_pointer1, T PPlib::ParticleData::* member_pointer2, BaseFileOutput *obj)
        : name(name), member_pointer0(member_pointer0), member_pointer1(member_pointer1), member_pointer2(member_pointer2), base(obj) {}

      ~VTK_DataArrayVectorByASCII()
      {
        delete base;
      }
      void write(std::list<PPlib::ParticleData *> *Particles, std::string filename)
      {
        base->write(Particles, filename);
        std::ofstream ofs(filename.c_str(),std::ios::app);
        ofs <<"<DataArray ";
        ofs <<"type=\""<<get_type((*Particles->begin())->*member_pointer0)<<"\" ";
        ofs <<"Name=\""<<name<<"\" ";
        ofs <<"NumberOfComponents=\"3\" ";
        ofs <<"format=\"ascii\">"<<std::endl;
        for (std::list<PPlib::ParticleData *>::iterator it=Particles->begin();it!=Particles->end();++it)
        {
          ofs << (*it)->*member_pointer0 << " "<< (*it)->*member_pointer1 << " "<< (*it)->*member_pointer2 << " ";
        }
        ofs <<std::endl;
        ofs <<"</DataArray>"<<std::endl;
      }

    private:
      VTK_DataArrayVectorByASCII();
      BaseFileOutput * base;
      std::string name;
      T PPlib::ParticleData::* member_pointer0;
      T PPlib::ParticleData::* member_pointer1;
      T PPlib::ParticleData::* member_pointer2;
  };


}//end of namespace
#endif
