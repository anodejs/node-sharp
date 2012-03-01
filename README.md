# Loading .Net libraries in Node Js
This is a native nodejs extension which allows us to load dot-net assemblies, create objects and call methods on them.

## Based upon
[saary's node.net](https://github.com/saary/node.net) and [v8sharp](http://v8sharp.codeplex.com/)

## The interface
Instantiate the module, load a dll and create a class.  Currently it searches the active directory of the node process for assemblies.

```javascript
var sharp = require('./Sharp').Sharp();
var stub = sharp.load('StubClass.dll');
var oc = stub.new('StubClass.Output');
```

Then the class exposes all methods for synchronous and asynchronous callbacks.

```javascript
var list = oc.call('ReturnList');
// list = [17, 6, 81]  // some random set of integers
```

```javascript
oc.async('ReturnObject', function(err, data){ 
    console.dir(data); 
});
```

There is also a .methods() call to return a list of callable method names.  

## What is it for?
Some of the linked resources from node.net, and research on the subject will tell you that C++/CLI has a performance
hit in node over native javascript.  This add-on uses System.Reflection for most operations, and additional marshaling
into managed types which further add to the cost.  So in short - your production system probably shouldn't rely on this
path.  It is intended for prototyping, when you already have classes to do what you want in C#.  And the same advice
as native modules apply - if your calls are long and async, then the costs of transforming the data are a smaller fraction.

## Things that work
* Listing dlls in the current directory
* Loading managed assemblies (doesn't have to be .dll, loading Sharp.node has all the fun generated internals)
* Call zero argument constructors
* Calling methods where types can be converted

Speaking of - type conversion (mostly inherited from v8sharp)

* string
* int
* date -> datetime
* arrays -> typed arrays and lists
* object -> dictionary of string and objects, and objects with public properties.
* returned objects are returned 

## What's next?
From the previous list, there are many things which could be added to make the process a little more seamless.

* Allow constructor arguments
* Static methods
* access to fields
* overloaded methods (honestly, this is hard when you're not a compiler)

* also playing with node-gyp and npm to get the build install to be more streamlined. 