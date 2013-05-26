#ifndef _bePimpl_h_
#define _bePimpl_h_

#include <new>

#define BE_PIMPL_CREATE(_class, ...)\
	private:\
		class Impl; Impl* m_impl;\
	public:\
		static _class* Create(__VA_ARGS__);\
		static void Destroy(_class* self);\
	

#define BE_PIMPL_CPP_DECLARE(_class, ...) \
	void _class::Destroy(_class* self){ self->m_impl->~Impl(); free(self); }\
	_class* _class::Create(__VA_ARGS__)

#define BE_PIMPL_CPP_BODY(_class, ...)\
	_class* self = (_class*)malloc(sizeof(_class) + sizeof(Impl));\
	void* impl = self+1;\
	new(impl) Impl(__VA_ARGS__);\
	self->m_impl = (Impl*)impl;\
	return self;

#endif // _bePimpl_h_
