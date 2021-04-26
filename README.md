# class_of_in_cpp
There was question in some [site](https://4programmers.net/Forum/C_i_C++/351347-czy_istnieje_w_c_odpowiednik_pascalowego_class_of), if it is possible to provide `class of` functionality from Pascal in C++.

[Pascal demo](https://ideone.com/b2Fa2N)
```pascal
{$MODE OBJFPC}{$LONGSTRINGS ON}
 
uses
  FGL;
 
type
  TCustomItem = class(TObject)
  public
    Name: String;
  end;
 
type
  TOrange = class(TCustomItem);
  TBanana = class(TCustomItem);
 
type
  TCustomItemClass = class of TCustomItem;
 
type
  TItems = specialize TFPGObjectList<TCustomItem>;
 
  function CreateItem(AClass: TCustomItemClass; const AName: String): TCustomItem;
  begin
    Result := AClass.Create();
    Result.Name := AName;
  end;
 
var
  Items: TItems;
  Item: TCustomItem;
begin
  Items := TItems.Create();
  try
    Items.Add(CreateItem(TOrange, 'orange'));
    Items.Add(CreateItem(TBanana, 'banana'));
    Items.Add(CreateItem(TCustomItem, 'custom'));
 
    for Item in Items do
      WriteLn(Item.Name);
  finally
    Items.Free();
  end;
end.
```

This is an attempt to provide such functionality.

First is is worth noticing that C++ functionalities are a bit different, what makes task a bit harder:
* no metadata or reflection
* subclasses do not inherit constructors by default
