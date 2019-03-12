namespace cpp ThriftService

service ThriftService
{
	void ping(),
	void hello(1:string msg),
	oneway void async_call()
}
