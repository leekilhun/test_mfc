/**
 * @file def_obj.hpp
 * @author lee kil hun (gns2.lee@)
 * @brief 
 * @details 생성할 object에 대한 instance index를 정의한다.
 * @version 0.1
 * @date 2024-12-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once
#ifndef __DEF_OBJ_HPP__
#define __DEF_OBJ_HPP__

#include "def_sys.hpp"

//-------------------------------------------------------
 //MARK: instance index
 //-------------------------------------------------------
  namespace INST_IDX
  {

    enum _IO : uint32_t
    {
      io_max
    };

    enum _BTN : uint32_t
    {
      btn_amx
    };

    enum _CYLINDER : uint32_t
    {
      cyl_max
    };

    enum _VACUUM : uint32_t
    {
      vac_max
    };

    enum _SERIAL: uint32_t
    {
      uart_api,
      uart_max
    };


    //tcp server
    enum _SOCKET : uint32_t
    {
      //net_nglog, //
      net_max
    };

    //tcp client socket
    enum _CSOCKET : uint32_t
    {
      cnet_max
    };

    //udp socket server
    enum _USOCKET : uint32_t
    {
      unet_max
    };

    //udp socket client
    enum _UCSOCKET : uint32_t
    {
      ucnet_loger, //loopback logging system
      ucnet_max
    };

    enum _LIB : uint32_t
    {
      lib_max
    };

    /*
      Motor-based object, conveyor, width adjustment, turn unit
      Functions - CW, CCW, Origin, Run, Stop
    */
    enum _EN_MOT : uint32_t
    {
      en_mot_max
    };



    /*
      Axis-based object (X, Y, Z, T, A, B, C)    
      Functions - Origin, Run, stop
    */
    enum _EN_AXIS : uint32_t
    {
      axis_max
    };

    enum _cnTASK : uint32_t
    {
      task_nglog,
      task_max
    };


    enum _CN : uint32_t
    {
      cn_oppanel,
      cn_automanager,
      cn_iomanager,
      cn_max
    };

    enum _API : uint32_t
    {
      api_cmd,
      api_max
    };


  }
  // end of obj

  namespace apcr
  {
    //MARK:flag control object idx
    /****************************************************
   ****************************************************/
    enum step_obj_idx
    {
      soi_max,
    };
  }
  // end of namespacce apcr

  namespace apcr
  {
    //-------------------------------------------------------
    //MARK:sys function prototype
    //-------------------------------------------------------
    using info_t = attribute_st;
    using obj_t  = ObjDataDef_t<ObjType>;

    //object instance data
    static std::vector<obj_t> obj_datas{};

    /**
     * @brief Get the object data object
     *  
     * @tparam retType 
     * @param idx 
     * @return retType 
     */
    template <typename retType>
    static retType get_object_data(uint32_t idx)
    {

      if constexpr (std::is_same_v<retType, obj_t *>)
      {
        for (auto &data : obj_datas)
        {
          if ((data.baseAddr + data.instanceNo) == idx)
            return &data;
        }
        return (obj_t *)nullptr;
      }
      else if constexpr (std::is_same_v<retType, info_t>)
      {
        info_t obj_info{};

        //std::cout << "get_object_data info_t : " << std::to_string(idx) << std::endl;
        for (auto &data : obj_datas)
        {
          if ((data.baseAddr + data.instanceNo) == idx)
          {
            obj_info.obj_type        = data.dataType;
            obj_info.object_no       = data.baseAddr;
            obj_info.instance_no     = data.instanceNo;
            obj_info.logKeeping_days = data.logKeepingDays;
            obj_info.obj_name        = data.objName;
            obj_info.logFile_name    = data.logFileName;
            return obj_info;
          }
        }
        return obj_info;
      }
      else
      {
        return retType{}; //error
      }
    }

    /**
     * @brief 
     *  object instance 등록한다.
     * @param idx 
     * @param ptr_obj 
     * @return errno_t 
     */
    static errno_t regist_object(uint32_t idx, IObj *ptr_obj)
    {
      obj_t *p_obj = get_object_data<obj_t *>(idx);
      if (p_obj == nullptr)
        return IObj::err_not_instanced;
      if (p_obj->dataType == ObjType::NONE)
        return IObj::err_not_found;

      p_obj->ptrObj = ptr_obj;
      p_obj->is_Reg = true;
      return ERROR_SUCCESS;
    }


  }
  // end of namespace apcr


#endif // __DEF_OBJ_HPP__
