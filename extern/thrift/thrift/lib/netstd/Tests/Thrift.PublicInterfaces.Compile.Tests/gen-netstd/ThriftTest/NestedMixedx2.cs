/**
 * Autogenerated by Thrift Compiler (0.14.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Extensions.Logging;
using Thrift;
using Thrift.Collections;
using System.ServiceModel;
using System.Runtime.Serialization;

using Thrift.Protocol;
using Thrift.Protocol.Entities;
using Thrift.Protocol.Utilities;
using Thrift.Transport;
using Thrift.Transport.Client;
using Thrift.Transport.Server;
using Thrift.Processor;


#pragma warning disable IDE0079  // remove unnecessary pragmas
#pragma warning disable IDE1006  // parts of the code use IDL spelling

namespace ThriftTest
{

[DataContract(Namespace="")]
public partial class NestedMixedx2 : TBase
{
  private List<THashSet<int>> _int_set_list;
  private Dictionary<int, THashSet<string>> _map_int_strset;
  private List<Dictionary<int, THashSet<string>>> _map_int_strset_list;

  [DataMember(Order = 0)]
  public List<THashSet<int>> Int_set_list
  {
    get
    {
      return _int_set_list;
    }
    set
    {
      __isset.int_set_list = true;
      this._int_set_list = value;
    }
  }

  [DataMember(Order = 0)]
  public Dictionary<int, THashSet<string>> Map_int_strset
  {
    get
    {
      return _map_int_strset;
    }
    set
    {
      __isset.map_int_strset = true;
      this._map_int_strset = value;
    }
  }

  [DataMember(Order = 0)]
  public List<Dictionary<int, THashSet<string>>> Map_int_strset_list
  {
    get
    {
      return _map_int_strset_list;
    }
    set
    {
      __isset.map_int_strset_list = true;
      this._map_int_strset_list = value;
    }
  }


  [DataMember(Order = 1)]
  public Isset __isset;
  [DataContract]
  public struct Isset
  {
    [DataMember]
    public bool int_set_list;
    [DataMember]
    public bool map_int_strset;
    [DataMember]
    public bool map_int_strset_list;
  }

  #region XmlSerializer support

  public bool ShouldSerializeInt_set_list()
  {
    return __isset.int_set_list;
  }

  public bool ShouldSerializeMap_int_strset()
  {
    return __isset.map_int_strset;
  }

  public bool ShouldSerializeMap_int_strset_list()
  {
    return __isset.map_int_strset_list;
  }

  #endregion XmlSerializer support

  public NestedMixedx2()
  {
  }

  public NestedMixedx2 DeepCopy()
  {
    var tmp141 = new NestedMixedx2();
    if((Int_set_list != null) && __isset.int_set_list)
    {
      tmp141.Int_set_list = this.Int_set_list.DeepCopy();
    }
    tmp141.__isset.int_set_list = this.__isset.int_set_list;
    if((Map_int_strset != null) && __isset.map_int_strset)
    {
      tmp141.Map_int_strset = this.Map_int_strset.DeepCopy();
    }
    tmp141.__isset.map_int_strset = this.__isset.map_int_strset;
    if((Map_int_strset_list != null) && __isset.map_int_strset_list)
    {
      tmp141.Map_int_strset_list = this.Map_int_strset_list.DeepCopy();
    }
    tmp141.__isset.map_int_strset_list = this.__isset.map_int_strset_list;
    return tmp141;
  }

  public async global::System.Threading.Tasks.Task ReadAsync(TProtocol iprot, CancellationToken cancellationToken)
  {
    iprot.IncrementRecursionDepth();
    try
    {
      TField field;
      await iprot.ReadStructBeginAsync(cancellationToken);
      while (true)
      {
        field = await iprot.ReadFieldBeginAsync(cancellationToken);
        if (field.Type == TType.Stop)
        {
          break;
        }

        switch (field.ID)
        {
          case 1:
            if (field.Type == TType.List)
            {
              {
                TList _list142 = await iprot.ReadListBeginAsync(cancellationToken);
                Int_set_list = new List<THashSet<int>>(_list142.Count);
                for(int _i143 = 0; _i143 < _list142.Count; ++_i143)
                {
                  THashSet<int> _elem144;
                  {
                    TSet _set145 = await iprot.ReadSetBeginAsync(cancellationToken);
                    _elem144 = new THashSet<int>(_set145.Count);
                    for(int _i146 = 0; _i146 < _set145.Count; ++_i146)
                    {
                      int _elem147;
                      _elem147 = await iprot.ReadI32Async(cancellationToken);
                      _elem144.Add(_elem147);
                    }
                    await iprot.ReadSetEndAsync(cancellationToken);
                  }
                  Int_set_list.Add(_elem144);
                }
                await iprot.ReadListEndAsync(cancellationToken);
              }
            }
            else
            {
              await TProtocolUtil.SkipAsync(iprot, field.Type, cancellationToken);
            }
            break;
          case 2:
            if (field.Type == TType.Map)
            {
              {
                TMap _map148 = await iprot.ReadMapBeginAsync(cancellationToken);
                Map_int_strset = new Dictionary<int, THashSet<string>>(_map148.Count);
                for(int _i149 = 0; _i149 < _map148.Count; ++_i149)
                {
                  int _key150;
                  THashSet<string> _val151;
                  _key150 = await iprot.ReadI32Async(cancellationToken);
                  {
                    TSet _set152 = await iprot.ReadSetBeginAsync(cancellationToken);
                    _val151 = new THashSet<string>(_set152.Count);
                    for(int _i153 = 0; _i153 < _set152.Count; ++_i153)
                    {
                      string _elem154;
                      _elem154 = await iprot.ReadStringAsync(cancellationToken);
                      _val151.Add(_elem154);
                    }
                    await iprot.ReadSetEndAsync(cancellationToken);
                  }
                  Map_int_strset[_key150] = _val151;
                }
                await iprot.ReadMapEndAsync(cancellationToken);
              }
            }
            else
            {
              await TProtocolUtil.SkipAsync(iprot, field.Type, cancellationToken);
            }
            break;
          case 3:
            if (field.Type == TType.List)
            {
              {
                TList _list155 = await iprot.ReadListBeginAsync(cancellationToken);
                Map_int_strset_list = new List<Dictionary<int, THashSet<string>>>(_list155.Count);
                for(int _i156 = 0; _i156 < _list155.Count; ++_i156)
                {
                  Dictionary<int, THashSet<string>> _elem157;
                  {
                    TMap _map158 = await iprot.ReadMapBeginAsync(cancellationToken);
                    _elem157 = new Dictionary<int, THashSet<string>>(_map158.Count);
                    for(int _i159 = 0; _i159 < _map158.Count; ++_i159)
                    {
                      int _key160;
                      THashSet<string> _val161;
                      _key160 = await iprot.ReadI32Async(cancellationToken);
                      {
                        TSet _set162 = await iprot.ReadSetBeginAsync(cancellationToken);
                        _val161 = new THashSet<string>(_set162.Count);
                        for(int _i163 = 0; _i163 < _set162.Count; ++_i163)
                        {
                          string _elem164;
                          _elem164 = await iprot.ReadStringAsync(cancellationToken);
                          _val161.Add(_elem164);
                        }
                        await iprot.ReadSetEndAsync(cancellationToken);
                      }
                      _elem157[_key160] = _val161;
                    }
                    await iprot.ReadMapEndAsync(cancellationToken);
                  }
                  Map_int_strset_list.Add(_elem157);
                }
                await iprot.ReadListEndAsync(cancellationToken);
              }
            }
            else
            {
              await TProtocolUtil.SkipAsync(iprot, field.Type, cancellationToken);
            }
            break;
          default: 
            await TProtocolUtil.SkipAsync(iprot, field.Type, cancellationToken);
            break;
        }

        await iprot.ReadFieldEndAsync(cancellationToken);
      }

      await iprot.ReadStructEndAsync(cancellationToken);
    }
    finally
    {
      iprot.DecrementRecursionDepth();
    }
  }

  public async global::System.Threading.Tasks.Task WriteAsync(TProtocol oprot, CancellationToken cancellationToken)
  {
    oprot.IncrementRecursionDepth();
    try
    {
      var struc = new TStruct("NestedMixedx2");
      await oprot.WriteStructBeginAsync(struc, cancellationToken);
      var field = new TField();
      if((Int_set_list != null) && __isset.int_set_list)
      {
        field.Name = "int_set_list";
        field.Type = TType.List;
        field.ID = 1;
        await oprot.WriteFieldBeginAsync(field, cancellationToken);
        {
          await oprot.WriteListBeginAsync(new TList(TType.Set, Int_set_list.Count), cancellationToken);
          foreach (THashSet<int> _iter165 in Int_set_list)
          {
            {
              await oprot.WriteSetBeginAsync(new TSet(TType.I32, _iter165.Count), cancellationToken);
              foreach (int _iter166 in _iter165)
              {
                await oprot.WriteI32Async(_iter166, cancellationToken);
              }
              await oprot.WriteSetEndAsync(cancellationToken);
            }
          }
          await oprot.WriteListEndAsync(cancellationToken);
        }
        await oprot.WriteFieldEndAsync(cancellationToken);
      }
      if((Map_int_strset != null) && __isset.map_int_strset)
      {
        field.Name = "map_int_strset";
        field.Type = TType.Map;
        field.ID = 2;
        await oprot.WriteFieldBeginAsync(field, cancellationToken);
        {
          await oprot.WriteMapBeginAsync(new TMap(TType.I32, TType.Set, Map_int_strset.Count), cancellationToken);
          foreach (int _iter167 in Map_int_strset.Keys)
          {
            await oprot.WriteI32Async(_iter167, cancellationToken);
            {
              await oprot.WriteSetBeginAsync(new TSet(TType.String, Map_int_strset[_iter167].Count), cancellationToken);
              foreach (string _iter168 in Map_int_strset[_iter167])
              {
                await oprot.WriteStringAsync(_iter168, cancellationToken);
              }
              await oprot.WriteSetEndAsync(cancellationToken);
            }
          }
          await oprot.WriteMapEndAsync(cancellationToken);
        }
        await oprot.WriteFieldEndAsync(cancellationToken);
      }
      if((Map_int_strset_list != null) && __isset.map_int_strset_list)
      {
        field.Name = "map_int_strset_list";
        field.Type = TType.List;
        field.ID = 3;
        await oprot.WriteFieldBeginAsync(field, cancellationToken);
        {
          await oprot.WriteListBeginAsync(new TList(TType.Map, Map_int_strset_list.Count), cancellationToken);
          foreach (Dictionary<int, THashSet<string>> _iter169 in Map_int_strset_list)
          {
            {
              await oprot.WriteMapBeginAsync(new TMap(TType.I32, TType.Set, _iter169.Count), cancellationToken);
              foreach (int _iter170 in _iter169.Keys)
              {
                await oprot.WriteI32Async(_iter170, cancellationToken);
                {
                  await oprot.WriteSetBeginAsync(new TSet(TType.String, _iter169[_iter170].Count), cancellationToken);
                  foreach (string _iter171 in _iter169[_iter170])
                  {
                    await oprot.WriteStringAsync(_iter171, cancellationToken);
                  }
                  await oprot.WriteSetEndAsync(cancellationToken);
                }
              }
              await oprot.WriteMapEndAsync(cancellationToken);
            }
          }
          await oprot.WriteListEndAsync(cancellationToken);
        }
        await oprot.WriteFieldEndAsync(cancellationToken);
      }
      await oprot.WriteFieldStopAsync(cancellationToken);
      await oprot.WriteStructEndAsync(cancellationToken);
    }
    finally
    {
      oprot.DecrementRecursionDepth();
    }
  }

  public override bool Equals(object that)
  {
    if (!(that is NestedMixedx2 other)) return false;
    if (ReferenceEquals(this, other)) return true;
    return ((__isset.int_set_list == other.__isset.int_set_list) && ((!__isset.int_set_list) || (TCollections.Equals(Int_set_list, other.Int_set_list))))
      && ((__isset.map_int_strset == other.__isset.map_int_strset) && ((!__isset.map_int_strset) || (TCollections.Equals(Map_int_strset, other.Map_int_strset))))
      && ((__isset.map_int_strset_list == other.__isset.map_int_strset_list) && ((!__isset.map_int_strset_list) || (TCollections.Equals(Map_int_strset_list, other.Map_int_strset_list))));
  }

  public override int GetHashCode() {
    int hashcode = 157;
    unchecked {
      if((Int_set_list != null) && __isset.int_set_list)
      {
        hashcode = (hashcode * 397) + TCollections.GetHashCode(Int_set_list);
      }
      if((Map_int_strset != null) && __isset.map_int_strset)
      {
        hashcode = (hashcode * 397) + TCollections.GetHashCode(Map_int_strset);
      }
      if((Map_int_strset_list != null) && __isset.map_int_strset_list)
      {
        hashcode = (hashcode * 397) + TCollections.GetHashCode(Map_int_strset_list);
      }
    }
    return hashcode;
  }

  public override string ToString()
  {
    var sb = new StringBuilder("NestedMixedx2(");
    int tmp172 = 0;
    if((Int_set_list != null) && __isset.int_set_list)
    {
      if(0 < tmp172++) { sb.Append(", "); }
      sb.Append("Int_set_list: ");
      Int_set_list.ToString(sb);
    }
    if((Map_int_strset != null) && __isset.map_int_strset)
    {
      if(0 < tmp172++) { sb.Append(", "); }
      sb.Append("Map_int_strset: ");
      Map_int_strset.ToString(sb);
    }
    if((Map_int_strset_list != null) && __isset.map_int_strset_list)
    {
      if(0 < tmp172++) { sb.Append(", "); }
      sb.Append("Map_int_strset_list: ");
      Map_int_strset_list.ToString(sb);
    }
    sb.Append(')');
    return sb.ToString();
  }
}

}
