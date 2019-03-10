namespace cpp Service

service Service
{
	void ping(),
	void hello(1:string msg),
	oneway void async_call()
}
