#pragma once

#define AuraConcatInternal(A, B) A##B
#define AuraConcat(A, B) AuraConcatInternal(A, B)

#define AuraScopeExit(...) ::Aura::ScopeExit AuraConcat(scopeExit, __LINE__) = [__VA_ARGS__]

#define AuraHandle(Type) using AuraConcat(Type, Handle) = ::Aura::Handle<struct Type>
#define AuraHandleImpl(Type) template<> struct Aura::Handle<Type>::Impl

#define AuraStackPoint() AuraScopeExit(head = ::Aura::LargeStack.Head) { ::Aura::LargeStack.Head = head; }
#define AuraStackAlloc(Type, Count) ::Aura::LargeStack.Allocate<Type>(Count)